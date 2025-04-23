#include <mpi.h>
#include <string>
#include <cstring>
#include <cstdio>

#define CHECK(fn) {int errcode; errcode = (fn); if (errcode != MPI_SUCCESS) handle_error(errcode, NULL); }

static void handle_error(int errcode, char *str)
{
        char msg[MPI_MAX_ERROR_STRING];
        int resultlen;
        MPI_Error_string(errcode, msg, &resultlen);
        fprintf(stderr, "%s: %s\n", str, msg);
        MPI_Abort(MPI_COMM_WORLD, 1);
}

int main(int argc, char **argv)
{
    int rank, nprocs;
    MPI_Status status;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    char recv_buf[1024];

    const int tag=999;

    // this passes:
    memset(recv_buf, 0, sizeof(recv_buf));
    std::string  hi = "Hello string type";
    if (rank == 0) {
	CHECK(MPI_Send(hi.c_str(), hi.size()+1, MPI_CHAR, 1, tag, MPI_COMM_WORLD));
	fprintf(stderr, "[%d of %d] succesfully sent from string type\n", rank, nprocs);
    } else if (rank == 1) {
	CHECK(MPI_Recv(recv_buf, 1024, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status));
	fprintf(stderr, "[%d of %d] successfuly recv %s\n", rank, nprocs, recv_buf);
    }

    // fails with a cxi error
    memset(recv_buf, 0, sizeof(recv_buf));
    const char *msg = "Hello stack";
    if (rank == 0) {
	CHECK(MPI_Send(msg, strlen(msg)+1, MPI_CHAR, 1, tag, MPI_COMM_WORLD));
	fprintf(stderr, "[%d of %d] successfully sent from stack\n", rank, nprocs);
    } else if (rank == 1) {
	CHECK(MPI_Recv(recv_buf, 1024, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status));
	fprintf(stderr, "[%d of %d] successfuly recv %s\n", rank, nprocs, recv_buf);
    }

    // sending a string literal will also fail with a cxi error
    memset(recv_buf, 0, sizeof(recv_buf));
    if (rank == 0) {
	CHECK(MPI_Send("hello", strlen("hello")+1, MPI_CHAR, 1, tag, MPI_COMM_WORLD));
	fprintf(stderr, "[%d of %d] successfully sent string literal\n", rank, nprocs);
    } else if (rank == 1) {
	CHECK(MPI_Recv(recv_buf, 1024, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status));
	fprintf(stderr, "[%d of %d] successfully recv %s\n", rank, nprocs, recv_buf);
    }
    MPI_Finalize();
}
