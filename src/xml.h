#ifndef DC_XML_H
#define DC_XML_H

#include "config.h"

#ifdef ETYMON_AF_XML

#include "index.h"
#include "fdef.h"
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/SAXParser.hpp>
/* #include <xercesc/internal/XMLScanner.hpp> */
#include <iostream>


using namespace xercesc;

/* using namespace std; */


#undef BELIST_USEMEM


#ifdef af__cplusplus
extern "C" {
#endif

	int dc_xml_init(ETYMON_AF_DC_INIT* dc_init);
	int dc_xml_index(ETYMON_AF_DC_INDEX* dc_index);

#ifdef af__cplusplus
}
#endif


typedef struct DC_XML_BSTACK_STRUCT {
	int elemid;
	unsigned int begin;
	DC_XML_BSTACK_STRUCT* next;
} DC_XML_BSTACK;

typedef struct DC_XML_BELIST_STRUCT {
	int elemid;
	int parent;
	unsigned int begin;
	unsigned int end;
#ifdef BELIST_USEMEM
	DC_XML_BELIST_STRUCT* next;
#endif
} DC_XML_BELIST;


/* class AttributeList; */

class DcXmlHandlers : public HandlerBase {
 public:
	DcXmlHandlers();
	void init(ETYMON_AF_DC_INDEX* dc_index);
	void finish();
	~DcXmlHandlers();

	bool getSawErrors() const {
		return fSawErrors;
	}

	void startElement(const XMLCh* const name, AttributeList& attributes);
	void endElement(const XMLCh* const name);
	void characters(const XMLCh* const chars, const unsigned int length);

	void warning(const SAXParseException& exception);
	void error(const SAXParseException& exception);
	void fatalError(const SAXParseException& exception);
	void resetErrors();

	void setParser(SAXParser* parser);
	
 private:
	void addWords(char* text);
	unsigned int getSrcOffset();
	unsigned int getPrevSrcOffset();
	
	bool            fSawErrors;
	
	SAXParser* _parser;
	unsigned int _prev_offset;
	DC_XML_BSTACK* _bstack_head;
	int _bstack_n;
#ifdef BELIST_USEMEM
	DC_XML_BELIST* _belist_head;
	DC_XML_BELIST* _belist_tail;
#else
	char _belist_fn[ETYMON_MAX_PATH_SIZE];  /* temporary until we integrate this with
						   the built-in file types */
	FILE* _belist_fp;
	DC_XML_BELIST _belist_node;
#endif	
	ETYMON_AF_DC_INDEX* _dc_index;
	ETYMON_AF_INDEX_ADD_DOC _add_doc;
	int _elem_n;
	int _max_doc_res;  /* maximum number of levels to descend for
			      purposes of defining document boundaries */
	int _doc_res_overflow;  /* how many levels over maximum we
				   have descended */
	uint4 _next_doc_id;
	ETYMON_AF_INDEX_ADD_WORD _add_word;
	ETYMON_AF_FDEF_RESOLVE_FIELD _resolve_field;
	uint2 _fields[ETYMON_MAX_FIELD_NEST];
	int _field_len;
};


#endif

#endif
