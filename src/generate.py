"""Generate the instruction classes

Usage:
    ./generate.py INPUTFILE OUTPUTFILE
"""

from docopt import docopt
import yaml
from copy import deepcopy
import collections

arguments = docopt(__doc__)

template = """
class {class_prefix}{insn_name} : public Instruction
{{
public:
    unsigned execute(TargetMachineBase *_mach, Slot** slots)
    {{
        // TODO assert enough slots
        // TODO assert slot types are RegisterSlots

        {machine_type_name} *mach = static_cast<{machine_type_name}*>(_mach);
        {reg_read}

        {code}

        {reg_write}

        return {n_slots};
    }}

    // Return the number and types of register slots required
    //    which ones are read and written
    std::vector<Slot*> getSlots()
    {{
        return {{ {slots} }};
    }}

    unsigned getNumberOfSlots() override
    {{
        return {n_slots};
    }}

    std::string toString()
    {{
        std::string output;
        iof::stringizer ss("{format}");

        for(unsigned i = 0; i < iof::countMarkers("{format}"); ++i)
        {{
            ss << "S" + std::to_string(i+1);
        }}

        return ss;
    }}

    std::string toString(Slot** slots)
    {{
        std::string output;
        iof::stringizer ss("{format}");

        for(unsigned i = 0; i < iof::countMarkers("{format}"); ++i)
        {{
            ss << slots[i];
        }}

        return ss;
    }}

    std::string getName()
    {{
        return "{print_name}";
    }}
}};
"""

factory_template = """
std::vector<std::function<Instruction *()>> {class_prefix}getAutogeneratedInstructionFactories()
{{
    std::vector<std::function<Instruction* ()>> factories;

    {factories}

    return factories;
}}

"""

factory_lambda_template = """
    factories.push_back([] {{ return new {class_prefix}{insn_name}();}});
"""

# Recursively merge dictionaries, giving priority to params. If a list
# is encountered, we expect that there will be one entry, and that will
# provide default values for every item in that list.
def apply_defaults(params, defaults):
    try:
        if params is None:
            params = defaults
        if type(params) in [str, unicode, int, long, float]:
            pass
        elif isinstance(params, list):
            # If there is a default entry, the list must be length one
            # and then we apply those defaults to the parameters
            if isinstance(defaults, list):
                if len(defaults) > 1:
                    raise ValueError("More than one defaults in list")
                for item in params:
                    apply_defaults(item, defaults[0])
            else:
                pass
        elif isinstance(params, dict):
            if isinstance(defaults, dict):
                for key in defaults:
                    if key in params:
                        params[key] = apply_defaults(params[key], defaults[key])
                    else:
                        params[key] = defaults[key]
            else:
                raise ValueError("Params dictionary met with non-dictionary default: {}, {}".format(params, defaults))
        else:
            raise RuntimeException("Unable to apply defaults to {} (from {})".format(params, defaults))
    except TypeError, e:
        raise TypeError("TypeError: Unable to apply defaults to {} (from {})".format(params, defaults))
    return params

with open(arguments["INPUTFILE"]) as f:
    obj = yaml.load(f)

machine = obj["machine"]
register_type = machine["register_type"]
register_slot_type = machine["register_slot_type"]
register_class_type = machine["register_class_type"]
class_prefix = machine["class_prefix"]
machine_type_name = machine["machine_type_name"]

with open(arguments["OUTPUTFILE"], "w") as fout:
    factories = []
    for insn_name, params_ in obj["instructions"].items():
        defaults = deepcopy(obj["defaults"])
        params = apply_defaults(params_, defaults)

        code = params['implementation']
        operands = params['operands']
        format = params['format']

        print_name = params['print_name']
        reg_read = ""
        reg_write = ""
        slots = []
        for i, op in enumerate(operands):
            if op["type"] == "RegisterSlot":
                if "r" in op["modifier"]:
                    reg_read += "{} r{} = mach->getRegister(slots[{}]);\n".format(register_type, chr(65+i), i)
                else:
                    reg_read += "{} r{} = 0;\n".format(register_type, chr(65+i))

                if "w" in op["modifier"]:
                    reg_write += "mach->setRegister(slots[{}], r{});\n".format(i, chr(65+i))

                slots.append("new {}({}::{}, {}, {})".format(
                    register_slot_type,
                    register_class_type,
                    op["class"],
                    "true" if "w" in op["modifier"] else "false",
                    "true" if "r" in op["modifier"] else "false"))
            elif op["type"] == "ConstantSlot":
                ranges = []
                for r in op["ranges"]:
                    ranges.append("{{{}, {}}}".format(r["lower"], r["upper"]))

                reg_read += "{} c{} = slots[{}]->getValue();\n".format(register_type, chr(65+i), i)
                slots.append("new ConstantSlot({{{}}})".format(", ".join(ranges)))

        slots = ", ".join(slots)
        n_slots = len(operands)

        s = template.format(**locals())

        fout.write(s)

        # Make generator lambdas for factories
        factories.append(factory_lambda_template.format(**locals()))
    fout.write(factory_template.format(class_prefix=class_prefix, factories="".join(factories)))

