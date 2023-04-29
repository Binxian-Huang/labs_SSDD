#include "servicios.h"
#include "claves.h"

bool_t
init_1_svc(int *result, struct svc_req *rqstp)
{
	bool_t retval;

	fprintf(stdout, "In init rpc server\n");
	if ((*result = init()) == -1) {
		fprintf(stderr, "Error in operation init server with value %d\n", *result);
		retval = FALSE;
	} else if (*result == 0) {
		fprintf(stdout, "Operation init correct in server with value %d\n", *result);
		retval = TRUE;
	} else {
		fprintf(stderr, "Unknown error in operation init server with value %d\n", *result);
		retval = FALSE;
	}

	return retval;
}

bool_t
set_1_svc(int key, char *value1, int value2, double value3, int *result,  struct svc_req *rqstp)
{
	bool_t retval;

	fprintf(stdout, "In set_value rpc server\n");
	if ((*result = set(key, value1, value2, value3)) == -1) {
		fprintf(stderr, "Error in operation set_value server with value %d\n", *result);
		retval = FALSE;
	} else if (*result == 0) {
		fprintf(stdout, "Operation set_value correct in server with value %d\n", *result);
		retval = TRUE;
	} else {
		fprintf(stderr, "Unknown error in operation set_value server with value %d\n", *result);
		retval = FALSE;
	}

	return retval;
}

bool_t
get_1_svc(int key, struct result_values *result,  struct svc_req *rqstp)
{
	bool_t retval;

	fprintf(stdout, "In get_value rpc server\n");
	if ((int operation = get(key, &result->value1, &result->value2, &result->value3)) == -1) {
		fprintf(stderr, "Error in operation get_value server with value %d\n", operation);
		retval = FALSE;
	} else if (*result == 0) {
		fprintf(stdout, "Operation get_value correct in server with value %d\n", operation);
		retval = TRUE;
	} else {
		fprintf(stderr, "Unknown error in operation get_value server with value %d\n", operation);
		retval = FALSE;
	}
	result->operation_result = operation;

	return retval;
}

bool_t
modify_1_svc(int key, char *value1, int value2, double value3, int *result,  struct svc_req *rqstp)
{
	bool_t retval;

	fprintf(stdout, "In modify_value rpc server\n");
	if ((*result = modify(key, value1, value2, value3)) == -1) {
		fprintf(stderr, "Error in operation modify_value server with value %d\n", *result);
		retval = FALSE;
	} else if (*result == 0) {
		fprintf(stdout, "Operation modify_value correct in server with value %d\n", *result);
		retval = TRUE;
	} else {
		fprintf(stderr, "Unknown error in operation modify_value server with value %d\n", *result);
		retval = FALSE;
	}

	return retval;
}

bool_t
delete_1_svc(int key, int *result,  struct svc_req *rqstp)
{
	bool_t retval;

	fprintf(stdout, "In delete_key rpc server\n");
	if ((*result = delete(key);) == -1) {
		fprintf(stderr, "Error in operation delete_key server with value %d\n", *result);
		retval = FALSE;
	} else if (*result == 0) {
		fprintf(stdout, "Operation delete_key correct in server with value %d\n", *result);
		retval = TRUE;
	} else {
		fprintf(stderr, "Unknown error in operation delete_key server with value %d\n", *result);
		retval = FALSE;
	}

	return retval;
}

bool_t
exist_1_svc(int key, int *result,  struct svc_req *rqstp)
{
	bool_t retval;

	fprintf(stdout, "In exist rpc server\n");
	if ((*result = exist(key)) == -1) {
		fprintf(stderr, "Error in operation exist server with value %d\n", *result);
		retval = FALSE;
	} else if (*result == 0 && *result == 1) {
		fprintf(stdout, "Operation exist correct in server with value %d\n", *result);
		retval = TRUE;
	} else {
		fprintf(stderr, "Unknown error in operation exist server with value %d\n", *result);
		retval = FALSE;
	}

	return retval;
}

bool_t
copy_1_svc(int key1, int key2, int *result,  struct svc_req *rqstp)
{
	bool_t retval;

	fprintf(stdout, "In copy_key rpc server\n");
	if ((*result = copy(key1, key2)) == -1) {
		fprintf(stderr, "Error in operation copy_key server with value %d\n", *result);
		retval = FALSE;
	} else if (*result == 0) {
		fprintf(stdout, "Operation copy_key correct in server with value %d\n", *result);
		retval = TRUE;
	} else {
		fprintf(stderr, "Unknown error in operation copy_key server with value %d\n", *result);
		retval = FALSE;
	}
	
	return retval;
}

int
services_1_freeresult (SVCXPRT *transp, xdrproc_t xdr_result, caddr_t result)
{
	xdr_free (xdr_result, result);

	/*
	 * Insert additional freeing code here, if needed
	 */

	return 1;
}
