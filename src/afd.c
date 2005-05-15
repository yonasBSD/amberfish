#include "soapH.h"

struct Namespace namespaces[] = {
	{ "SOAP-ENV", "http://schemas.xmlsoap.org/soap/envelope/" },
	{ "SOAP-ENC", "http://schemas.xmlsoap.org/soap/encoding/" },
	{ "xsi", "http://www.w3.org/2001/XMLSchema-instance", "http://wwww.w3.org/*/XMLSchema-instance" },
	{ "xsd", "http://www.w3.org/2001/XMLSchema", "http://www.w3.org/*/XMLSchema" },
	{ "ns1", "urn:af" },
	{ NULL, NULL }
};

int ns1__test(struct soap *soap, char **s)
{
	*s = (char *) soap_strdup(soap, "Welcome to Amberfish.");
	return SOAP_OK;
}

int afdmain(int argc, char **argv)
{
	struct soap *soap;
	int child_socket;

	soap = soap_new();
	if (soap_bind(soap, "localhost", 8080, 100) < 0) {
		soap_print_fault(soap, stderr);
		return -1;
	}
	while (1) {
		if ((child_socket = soap_accept(soap)) < 0) {
			soap_print_fault(soap, stderr);
			return -1;
		}
		soap_serve(soap);
		soap_destroy(soap);
		soap_end(soap);
	}
}
