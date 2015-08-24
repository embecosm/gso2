#ifndef __MPI_HPP__
#define __MPI_HPP__

#include <mpi.h>
#include <functional>

#include "../algorithms/bruteforce.hpp"
#include "../algorithms/bruteforceByCost.hpp"

#define REQ_TASK        1
#define TASK            2

#define FINISHED        3

void distributeTasks(std::vector<std::vector<int>> &values,
    std::function<int(std::vector<std::vector<int>::iterator> &)> costFn)
{
    char buf[1];
    MPI_Status status;
    uint64_t total_size = 1;
    uint64_t current_size=0;

    std::vector<std::vector<int>::iterator> iterators;
    std::vector<int> iters_int(values.size());

    for(auto &v: values)
    {
        iterators.push_back(v.begin());
        total_size *= v.size();
    }

    bruteforceByCost<std::vector<int>> bfbc(values, iterators, costFn);

    do {
        MPI_Recv(buf, 1, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        for(unsigned i = 0; i < iterators.size(); ++i)
        {
            std::cout << *iterators[i] << " ";
            iters_int[i] = *iterators[i];
        }
            std::cout << "\n";

        MPI_Send(&iters_int[0], iters_int.size(), MPI_INT, status.MPI_SOURCE,
            TASK, MPI_COMM_WORLD);

        current_size++;
        std::cout << double(current_size) / double(total_size) *100.<< "%" << std::endl;
    } while(bfbc.next());

    // TODO wait for children to terminate
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    for(unsigned i = 1; i < world_size; ++i)
        MPI_Send(&buf, 1, MPI_CHAR, i,
            FINISHED, MPI_COMM_WORLD);
}

void distributeTasks(std::vector<std::vector<int>> &values)
{
    distributeTasks(values,
        [](std::vector<std::vector<int>::iterator> &iters)
        {
            int i = 0;

            for(auto &it: iters)
                i += *it;
            return -i;
        });
}

bool getWork(std::vector<int> &data)
{
    char buf[1] = {0};
    MPI_Status status;
    int count;

    MPI_Send(buf, 1, MPI_CHAR, 0, REQ_TASK, MPI_COMM_WORLD);

    MPI_Probe(0, MPI_ANY_TAG,  MPI_COMM_WORLD, &status);

    if(status.MPI_TAG == FINISHED)
    {
        return false;
    }

    MPI_Get_count(&status, MPI_INT, &count);

    data.resize(count);

    MPI_Recv(&data[0], count, MPI_INT, 0, TASK, MPI_COMM_WORLD, &status);

    return true;
}

#endif
