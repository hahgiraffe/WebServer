/**
 * @Description:
 * @Author: hahagiraffe
 * @Date: Created in 下午1:46 2019/8/5
**/
#ifndef TINYJSON_TYJSON_H
#define TINYJSON_TYJSON_H
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <string.h>


#define init(x) do{ (x)->type=TYPE_NULL; }while(0)
#define set_null(x) json_free(x)

/*声明json的7种类型*/
typedef enum {
    TYPE_NULL,
    TYPE_FALSE,
    TYPE_TRUE,
    TYPE_NUMBER,
    TYPE_STRING,
    TYPE_ARRAY,
    TYPE_OBJECT
}JsonType;

/*解析返回的状态,错误码
 * OK表示返回正常
 * EXPECT_VALUE表示json只含有空白
 * ROOT_NOT_SINGULAR表示一个值之后空白还有其他字符
 * INVALID_VALUE表示不合法返回
 * */
typedef enum {
    PARSE_OK=0,
    PARSE_EXPECT_VALUE,
    PARSE_INVALID_VALUE,
    PARSE_ROOT_NOT_SINGULAR,
    PARSE_NUMBER_TOO_BIG,
    PARSE_MISS_QUOTATION_MARK,
    PARSE_INVALID_STRING_ESCAPE,
    PARSE_INVALID_UNICODE_HEX,
    PARSE_INVALID_UNICODE_SURROGATE,
    PARSE_MISS_COMMA_OR_SQUARE_BRACKET,
    PARSE_MISS_KEY,
    PARSE_MISS_COLON,
    PARSE_MISS_COMMA_OR_CURLY_BRACKET,
    STRINGIFY_OK
}Parse_status;

/*Json数据的结构体
 * 因为不会同时为数字或字符串
 * 所以用union，32位机器上能省下4byte
 * */
typedef struct JsonValue JsonValue ;
typedef struct JsonMember JsonMember;

/*Json数据解析后缓存结构体*/
struct JsonValue{
    JsonType type;
    union {
        double number;/*数字*/
        struct {
            char* str;
            int len;
        }s;/*字符串*/

        struct {
            JsonValue* e;
            size_t size;
        }a;/*数组*/

        struct {
          JsonMember* m;
          size_t  size;
        }o;/*对象*/
    }u;
};

struct JsonMember{
    char* key;
    JsonValue value;
    size_t keylen;
};

/*Json数据传入时结构体*/
typedef struct {
    const char* json;
    char* stack;
    size_t size,top;
}JsonContext;

/*Json解析器*/
int json_parse(JsonValue* value,const char* json);

/*Json生成器*/
int json_stringify(const JsonValue* v,char** json,size_t* len);

/*Json解析string*/
int parse_string(JsonContext* c, JsonValue* v);

/*Json解析array*/
int parse_array(JsonContext* c, JsonValue* v);

/*Json解析分类*/
int parse_value(JsonContext* context,JsonValue* value);

/*Json解析object*/
int parse_object(JsonContext* c,JsonValue* v);

/*返回Json的type*/
JsonType get_type(const JsonValue* value);

/*如果Json的类型是number则返回值*/
double get_number(const JsonValue* value);

/*如果Json的类型是number则设置值*/
void set_number(JsonValue* value,double num);

/*如果Json类型是string则设置string分配内存*/
void set_string(JsonValue* value,const char* s,size_t len);

/*如果Json的类型是string则返回string长度*/
size_t get_string_len(JsonValue* value);

/*如果Json的类型是string则返回string值*/
const char* get_string(JsonValue* value);

/*释放内存*/
void json_free(JsonValue* value);

/*如果Json的类型是array则返回array长度*/
size_t get_array_size(JsonValue* value);

/*如果Json的类型是array则返回array值*/
JsonValue* get_array_value(JsonValue* value,size_t index);

/*如果Json的类型是object则返回对象大小*/
size_t get_object_size(const JsonValue* value);

/*如果Json的类型是对象则返回key*/
const char* get_object_key(const JsonValue* value,size_t index);

/*如果Json的类型是对象则返回key长度*/
size_t get_object_keylen(const JsonValue* value,size_t index);

/*如果Json的类型是对象则返回value*/
JsonValue* get_object_value(const JsonValue* value,size_t index);

#endif /*TINYJSON_TYJSON_H*/
