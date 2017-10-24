#include <pservlet.h>
#include <pstd.h>
#include <proto.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <libxml/xmlreader.h>
#include <json.h>
#include <json_tokener.h>
enum {
	RAW,
	XML,
	JSON,
	PROTO,
	SINK
};

typedef struct {
	pipe_t in, out;
	int flags;
	pstd_type_model_t* tm;
	struct {
		pstd_type_accessor_t x0,y0,x1,y1,x2,y2;
	} accessor;
	uint64_t _ts[65536];
	uint32_t _tsc;
	const char* name;
} ctx_t;

typedef struct {
	double x0,y0,x1,y1,x2,y2;
} triangle_t;

static inline void record_time(ctx_t* ctx)
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ctx->_ts[ctx->_tsc] = ts.tv_sec * 1000000000ull;
	ctx->_ts[ctx->_tsc] += ts.tv_nsec;
	ctx->_tsc ++;
}

static int _init(uint32_t argc, char const* const* argv, void* ctxmem)
{
	ctx_t* ctx = (ctx_t*)ctxmem;
	const char* typename = "";

	if(strcmp(argv[2], "raw") == 0)
		ctx->flags = RAW, typename = NULL;
	else if(strcmp(argv[2], "xml") == 0)
		ctx->flags = XML, typename = NULL;
	else if(strcmp(argv[2], "json") == 0)
		ctx->flags = JSON, typename = NULL;
	else if(strcmp(argv[2], "proto") == 0)
		ctx->flags = PROTO, typename = "evaluation/Triangle";
	
	if(strcmp(argv[1], "sink") == 0)
		ctx->flags |= SINK, ctx->name = "sink";
	else 
		ctx->name = "source";


	ctx->in = pipe_define("in", PIPE_INPUT, (ctx->flags & SINK) ? typename : NULL);
	ctx->out = pipe_define("out", PIPE_OUTPUT, (ctx->flags & SINK) ? NULL : typename);

	ctx->tm = pstd_type_model_new();

	if(typename != NULL)
	{
		pipe_t pipe = (ctx->flags & SINK) ? ctx->in : ctx->out;
		ctx->accessor.x0 = pstd_type_model_get_accessor(ctx->tm, pipe, "x0");
		ctx->accessor.y0 = pstd_type_model_get_accessor(ctx->tm, pipe, "y0");
		ctx->accessor.x1 = pstd_type_model_get_accessor(ctx->tm, pipe, "x1");
		ctx->accessor.y1 = pstd_type_model_get_accessor(ctx->tm, pipe, "y1");
		ctx->accessor.x2 = pstd_type_model_get_accessor(ctx->tm, pipe, "x2");
		ctx->accessor.y2 = pstd_type_model_get_accessor(ctx->tm, pipe, "y2");
	}
	
	ctx->_tsc = 0;

	return 0;
}

static inline int to_raw(pipe_t out, double x0, double y0, double x1, double y1, double x2, double y2)
{
	triangle_t t = {
		.x0 = x0,
		.y0 = y0,
		.x1 = x1,
		.y1 = y1,
		.x2 = x2,
		.y2 = y2
	};

	pipe_write(out, &t, sizeof(t));

	return 0;
}

static inline  int to_json(pipe_t out, double x0, double y0, double x1, double y1, double x2, double y2)
{
	char buf[1024];
	int n = snprintf(buf, sizeof(buf), "{\"x0\":%f, \"y0\":%f, \"x1\":%f, \"y1\":%f, \"x2\":%f, \"y2\":%f}", x0, y0,x1,y1,x2,y2);
	pipe_write(out, buf, n + 1);
	return 0;
}

static inline  int to_xml(pipe_t out, double x0, double y0, double x1, double y1, double x2, double y2)
{
	char buf[1024];
	int n = snprintf(buf, sizeof(buf), "<triangle>\n<x0>%f</x0>\n<y0>%f</y0>\n<x1>%f</x1>\n<y1>%f</y1>\n<x2>%f</x2>\n<y2>%f</y2>\n</triangle>", x0, y0,x1,y1,x2,y2);
	pipe_write(out, buf, n + 1);
	return 0;
}

static inline  int to_proto(ctx_t* ctx, pstd_type_instance_t* ti, double x0, double y0, double x1, double y1, double x2, double y2)
{
	PSTD_TYPE_INST_WRITE_PRIMITIVE(ti, ctx->accessor.x0, x0);
	PSTD_TYPE_INST_WRITE_PRIMITIVE(ti, ctx->accessor.y0, y0);
	PSTD_TYPE_INST_WRITE_PRIMITIVE(ti, ctx->accessor.x1, x1);
	PSTD_TYPE_INST_WRITE_PRIMITIVE(ti, ctx->accessor.y1, y1);
	PSTD_TYPE_INST_WRITE_PRIMITIVE(ti, ctx->accessor.x2, x2);
	PSTD_TYPE_INST_WRITE_PRIMITIVE(ti, ctx->accessor.y2, y2);
	return 0;
}

static inline int from_proto(ctx_t* ctx, pstd_type_instance_t* ti, double* x0, double* y0, double* x1, double* y1, double* x2, double* y2)
{
	*x0 = PSTD_TYPE_INST_READ_PRIMITIVE(double, ti, ctx->accessor.x0);
	*y0 = PSTD_TYPE_INST_READ_PRIMITIVE(double, ti, ctx->accessor.y0);
	*x1 = PSTD_TYPE_INST_READ_PRIMITIVE(double, ti, ctx->accessor.x1);
	*y1 = PSTD_TYPE_INST_READ_PRIMITIVE(double, ti, ctx->accessor.y1);
	*x2 = PSTD_TYPE_INST_READ_PRIMITIVE(double, ti, ctx->accessor.x2);
	*y2 = PSTD_TYPE_INST_READ_PRIMITIVE(double, ti, ctx->accessor.y2);
	return 0;
}

static inline int from_raw(pipe_t in,  double* x0, double* y0, double* x1, double* y1, double* x2, double* y2)
{
	triangle_t buf;
	pipe_read(in, &buf, sizeof(buf));

	*x0 = buf.x0; 
	*y0 = buf.y0; 
	*x1 = buf.x1; 
	*y1 = buf.y1; 
	*x2 = buf.x2; 
	*y2 = buf.y2; 

	return 0;
}

static inline int from_xml(pipe_t in,  double* x0, double* y0, double* x1, double* y1, double* x2, double* y2)
{
	char buf[1024];
	pipe_read(in, buf, sizeof(buf));
	xmlDocPtr xml_obj = xmlParseDoc((xmlChar*)buf);
	xmlNodePtr xml_root = xmlDocGetRootElement(xml_obj);

	xmlNodePtr cur = xml_root->xmlChildrenNode;

	while(NULL != cur)
	{
		double* p = NULL;
		if(strcmp((char*)cur->name, "x0") == 0) p = x0;
		if(strcmp((char*)cur->name, "y0") == 0) p = y0;
		if(strcmp((char*)cur->name, "x1") == 0) p = x1;
		if(strcmp((char*)cur->name, "y1") == 0) p = y1;
		if(strcmp((char*)cur->name, "x2") == 0) p = x2;
		if(strcmp((char*)cur->name, "y2") == 0) p = y2;
		if(p != NULL)
			sscanf((char*)xmlNodeListGetString(xml_obj, cur->xmlChildrenNode, 1), "%lf", p);
		cur = cur->next;
	}

	return 0;
}

static inline int from_json(pipe_t in, double* x0, double* y0, double* x1, double* y1, double* x2, double* y2)
{
	char buf[1024];
	buf[pipe_read(in, buf, sizeof(buf))] =  0;
	json_object* obj = json_tokener_parse(buf);
	json_object* ox0 = json_object_object_get(obj, "x0");
	json_object* ox1 = json_object_object_get(obj, "x1");
	json_object* ox2 = json_object_object_get(obj, "x2");
	json_object* oy0 = json_object_object_get(obj, "y0");
	json_object* oy1 = json_object_object_get(obj, "y1");
	json_object* oy2 = json_object_object_get(obj, "y2");

	*x0 = json_object_get_double(ox0);
	*x1 = json_object_get_double(ox1);
	*x2 = json_object_get_double(ox2);
	*y0 = json_object_get_double(oy0);
	*y1 = json_object_get_double(oy1);
	*y2 = json_object_get_double(oy2);

	json_object_put(obj);

	return 0;
}


static int _exec(void* ctxmem)
{
	ctx_t* ctx = (ctx_t*)ctxmem;
	size_t ti_size = pstd_type_instance_size(ctx->tm);
	char buf[ti_size];
	pstd_type_instance_t* ti = pstd_type_instance_new(ctx->tm, buf);

	double x0, y0, x1, y1, x2, y2;

	if(ctx->flags & SINK)
	{
		switch(ctx->flags & 3)
		{
			case RAW:
				from_raw(ctx->in, &x0, &y0, &x1, &y1, &x2, &y2);
				break;
			case JSON:
				from_json(ctx->in, &x0, &y0, &x1, &y1, &x2, &y2);
				break;
			case XML:
				from_xml(ctx->in, &x0, &y0, &x1, &y1, &x2, &y2);
				break;
			case PROTO:
				from_proto(ctx, ti, &x0,&y0,&x1,&y1,&x2,&y2);
				break;
		}
		record_time(ctx);

		char buf[1024];
		int n = snprintf(buf, sizeof(buf), "%lf%lf%lf%lf%lf%lf", x0, y0, x1, y1, x2, y2);
		pipe_write(ctx->out, buf, n);
	}
	else
	{
		char buf[1024];
		buf[pipe_read(ctx->in, buf, sizeof(buf))] = 0;
		sscanf(buf, "%lf%lf%lf%lf%lf%lf", &x0, &y0, &x1, &y1, &x2, &y2);

		record_time(ctx);
		switch(ctx->flags & 3)
		{
			case RAW:
				to_raw(ctx->out, x0, y0, x1, y1, x2, y2);
				break;
			case JSON:
				to_json(ctx->out, x0, y0, x1, y1, x2, y2);
				break;
			case XML:
				to_xml(ctx->out, x0, y0, x1, y1, x2, y2);
				break;
			case PROTO:
				to_proto(ctx, ti, x0,y0,x1,y1,x2,y2);
				break;
		}
	}

	pstd_type_instance_free(ti);

	return 0;
}

static int _unload(void* ctxmem)
{
	ctx_t* ctx = (ctx_t*)ctxmem;
	pstd_type_model_free(ctx->tm);
	uint32_t i;
	for(i = 0; i < ctx->_tsc; i ++)
		printf("%s[%u] = %"PRIu64"\n", ctx->name, i, ctx->_ts[i]);
	return 0;
}

SERVLET_DEF = {
	.desc = "Test",
	.version = 0x0,
	.size = sizeof(ctx_t),
	.init = _init,
	.exec = _exec,
	.unload = _unload
};
