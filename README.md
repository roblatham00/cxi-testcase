# Slingshot (cxi) memory error reproducer

It all started with an unusual DAOS error

https://daosio.atlassian.net/browse/DAOS-17004

but it turns out the error doesn't have anything to do with DAOS after all

In this test case, using only MPI routines, we can trigger the same error
( `cxil_map: write error`) by asking CXI (through libfabric) to transfer
certain kinds of memory regions.

## compiling

    mpicxx cxi-testcase.cxx -o cxi-testcase` -- doesn't need any additional libraries beyond MPI

## Running

It's a simple two process send/receive test.  I have a PBS job script suitable
for ALCF Aurora.  Pretty sure you'll need two nodes, otherwise a shared-memory
optimization will bypass the CXI code.

    mpiexec -np 2 ./cxi-testcase


## Environment variables

I've tried a few things:

- `FI_LOG_LEVEL=debug` : will give you tons of information confirming it's CXI mishandling this kind of memory
- `MPIR_CVAR_CH4_OFI_ENABLE_INJECT=0` : take a less-optimized path to send the data, bypassing libfabrics `fi_tinject` code.  Doesn't matter, though: still get the same CXI error
- `FI_CXI_DISABLE_NON_INJECT_MSG_IDC` : you can set this to 0 or 1 it won't make any difference
