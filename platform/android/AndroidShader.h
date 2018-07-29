/*
 * Simple shader helper based on: http://learnopengl.com/code_viewer.php?type=header&code=shader
 */
#pragma once

#include <GLES3/gl3.h>
#include <EGL/egl.h>

#include "Shader.h"

#define STR(s) #s
#define STRV(s) STR(s)

#define POS_ATTRIB 0
#define COLOR_ATTRIB 1
#define SCALEROT_ATTRIB 2
#define OFFSET_ATTRIB 3

static const char VERTEX_SHADER[] =
    "#version 300 es\n"
    "layout(location = " STRV(POS_ATTRIB) ") in vec2 pos;\n"
    "layout(location=" STRV(COLOR_ATTRIB) ") in vec4 color;\n"
    "layout(location=" STRV(SCALEROT_ATTRIB) ") in vec4 scaleRot;\n"
    "layout(location=" STRV(OFFSET_ATTRIB) ") in vec2 offset;\n"
    "out vec4 vColor;\n"
    "void main() {\n"
    "    mat2 sr = mat2(scaleRot.xy, scaleRot.zw);\n"
    "    gl_Position = vec4(sr*pos + offset, 0.0, 1.0);\n"
    "    vColor = color;\n"
    "}\n";

static const char FRAGMENT_SHADER[] =
    "#version 300 es\n"
    "precision mediump float;\n"
    "in vec4 vColor;\n"
    "out vec4 outColor;\n"
    "void main() {\n"
    "    outColor = vColor;\n"
    "}\n";

static const char COLOR_VERTEX_SHADER[] =
        "#version 300 es\n"
                "layout (location = 0) in vec3 position;\n"
                "layout (location = 1) in vec3 normal;\n"
                "layout (location = 2) in vec3 color;\n"
                "layout (location = 3) in vec2 texCoords;\n"
                "layout (location = 4) in mat4 instanceMatrix;\n"
                "\n"
                "out vec2 TexCoords;\n"
                "out vec3 VertexColor;\n"
                "out vec3 VertexNormal;\n"
                "out vec3 FragPos;\n"
                "out vec4 FragPosLightSpace;\n"
                "\n"
                "//uniform mat4 model;\n"
                "uniform mat4 view;\n"
                "uniform mat4 projection;\n"
                "uniform mat4 lightSpaceMatrix;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    gl_Position = projection * view * instanceMatrix * vec4(position, 1.0f);\n"
                "\n"
                "    TexCoords = texCoords;\n"
                "    VertexColor = color;\n"
                "\tFragPos = vec3(instanceMatrix * vec4(position, 1.0f)); // vertex in world space\n"
                "\t//VertexNormal = mat3(transpose(inverse(instanceMatrix))) * normal;   // corrects scaling\n"
                "\tVertexNormal = mat3(transpose(inverse(instanceMatrix))) * normal;   // corrects scaling\n"
                "\n"
                "\tFragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0); // vertex in light space\n"
                "}\n";




static const char COLOR_FRAGMENT_SHADER[] =
        "#version 300 es\n"
                "\n"
                "precision highp float;\n"
                "precision highp int;"
                "in vec3 VertexColor;\n"
                "in vec3 VertexNormal;\n"
                "in vec3 FragPos;\n"
                "in vec4 FragPosLightSpace;\n"
                "\n"
                "out vec4 color;\n"
                "\n"
                "\n"
                "uniform vec3 lightPos;\n"
                "uniform vec3 lightColor;\n"
                "uniform vec3 viewPos; \n"
                "\n"
                "uniform sampler2D shadowMap;\n"
                "\n"
                "float CalcShadow(float bias)\n"
                "{\n"
                "\t// restore homogenous coordinates\n"
                "    vec3 projCoords = FragPosLightSpace.xyz / FragPosLightSpace.w;\t\n"
                "\tprojCoords = projCoords * 0.5 + 0.5;  // transform to [0,1] range\n"
                "\n"
                "\t//float closestDepth = texture(shadowMap, projCoords.xy).r; // depth from light at fragment coordinate\n"
                "\tfloat currentDepth = projCoords.z; // fragment depth projected in light space\n"
                "\n"
                "\t//float shadow = currentDepth - bias> closestDepth ? 1.0 : 0.0;\n"
                "\n"
                "\tfloat shadow = 0.0;\n"
                "\tvec2 texelSize = vec2( 1./ float(textureSize(shadowMap, 0).x), 1./ float(textureSize(shadowMap, 0).y)   );\n"
                "\tfor(int x = -1; x <= 1; ++x)\n"
                "\t{\n"
                "\t\tfor(int y = -1; y <= 1; ++y)\n"
                "\t\t{\n"
                "\t\t\tfloat pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; \n"
                "\t\t\tshadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        \n"
                "\t\t}    \n"
                "\t}\n"
                "\tshadow /= 9.0;\n"
                "\n"
                "\tif (projCoords.z > 1.0) // projection outside light frustum\n"
                "\t\tshadow = 0.0;\n"
                "\n"
                "\treturn shadow;\n"
                "}\n"
                "\n"
                "void main()\n"
                "{\n"
                "\t// ambient color\n"
                "\tfloat ambientStrength = 0.25f;\n"
                "\tvec3 ambient = ambientStrength * lightColor;\n"
                "\n"
                "\t// diffuse color\n"
                "\tvec3 norm = normalize(VertexNormal);\n"
                "\tvec3 lightDir = normalize(lightPos - FragPos); // vec from fragment to light source\n"
                "\tfloat diff = max(dot(norm,lightDir), 0.0); // determines how parallel the light is compared to the normal vector\n"
                "\tvec3 diffuse = diff * lightColor;\n"
                "\n"
                "\t// specular color\n"
                "\tvec3 viewDir = normalize(viewPos - FragPos);\n"
                "\tvec3 reflectDir = reflect(-lightDir, norm); // calculates reflected light direction\n"
                "\tfloat spec = pow(max(dot(viewDir, reflectDir), 0.0), 4.0); // increase if reflection is parallel to viewDir\n"
                "\tvec3 specular = spec*lightColor;\n"
                "\n"
                "\n"
                "\t//float bias = max(0.001 * (1.0 - dot(norm, lightDir)), 0.0005);  \n"
				"float dnl = dot(norm, lightDir); if (dnl < 0.0) dnl = 0.0; else if (dnl > 1.0) dnl = 1.0;\n"
				"\tfloat bias = 0.005 * tan(acos( dnl));\n"
                "\tfloat shadow = CalcShadow(bias);\n"
                "\tshadow = min(shadow, 0.8);\n"
                "\tvec3 brightness = (ambient + 1.0 - shadow) * (ambient + specular + diffuse);"
                "\tcolor = vec4(brightness * VertexColor, 1.0f);\n"
                "}";


static const char DEPTH_VERTEX_SHADER[] =
        "#version 300 es\n"
                "layout (location = 0) in vec3 position;\n"
                "layout (location = 4) in mat4 instanceMatrix;\n"
                "\n"
                "uniform mat4 lightSpaceMatrix;\n"
                "//uniform mat4 model;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    gl_Position = lightSpaceMatrix * instanceMatrix * vec4(position, 1.0f);\n"
                "}\n";

static const char DEPTH_FRAGMENT_SHADER[] =
        "#version 300 es\n"
                "\n"
                "// we only want to calculate the depth map (gl_FragDepth = gl_FragCoord.z) with is done by Z-buffer testing\n"
                "void main()\n"
                "{ \n"
                "\n"
                "} ";

static const char DEBUG_VERTEX_SHADER[] =
        "#version 300 es\n"
                "layout (location = 0) in vec3 position;\n"
                "layout (location = 1) in vec3 normal;\n"
                "layout (location = 2) in vec3 color;\n"
                "layout (location = 3) in vec2 texCoords;\n"
                "layout (location = 4) in mat4 instanceMatrix;\n"
                "\n"
                "out vec3 VertexColor;\n"
                "\n"
                "uniform mat4 view;\n"
                "uniform mat4 projection;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    gl_Position = projection * view * instanceMatrix * vec4(position, 1.0f);\n"
                "    VertexColor = color;\n"
                "}\n";

static const char DEBUG_FRAGMENT_SHADER[] =
        "#version 300 es\n"
                "\n"
                "in vec3 VertexColor;\n"
                "out vec4 color;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    color = vec4(VertexColor, 1.0f);\n"
                "}\n";



class AndroidShader : public Shader
{
public:


    // Constructor generates the shader on the fly
    AndroidShader(const char* vtxSrc, const char* fragSrc)
    {
        ALOGV("Create shader obj");
        Program = createProgram(vtxSrc, fragSrc);
    }


    bool checkGlError(const char* funcName) {
        GLint err = glGetError();
        if (err != GL_NO_ERROR) {
            ALOGE("GL error after %s(): 0x%08x\n", funcName, err);
            return true;
        }
        return false;
    }

    GLuint createShader(GLenum shaderType, const char* src) {

        ALOGV("Create shader");

        GLuint shader = glCreateShader(shaderType);
        if (!shader) {
            checkGlError("glCreateShader");
            return 0;
        }
        glShaderSource(shader, 1, &src, NULL);

        GLint compiled = GL_FALSE;
        glCompileShader(shader);
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLogLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
            if (infoLogLen > 0) {
                GLchar* infoLog = (GLchar*)malloc(infoLogLen);
                if (infoLog) {
                    glGetShaderInfoLog(shader, infoLogLen, NULL, infoLog);
                    ALOGE("Could not compile %s shader:\n%s\n",
                          shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment",
                          infoLog);
                    free(infoLog);
                }
            }
            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }

    GLuint createProgram(const char* vtxSrc, const char* fragSrc) {

        ALOGV("Create Program");

        GLuint vtxShader = 0;
        GLuint fragShader = 0;
        GLuint program = 0;
        GLint linked = GL_FALSE;

        vtxShader = createShader(GL_VERTEX_SHADER, vtxSrc);
        if (!vtxShader)
            goto exit;

        fragShader = createShader(GL_FRAGMENT_SHADER, fragSrc);
        if (!fragShader)
            goto exit;

        program = glCreateProgram();
        if (!program) {
            checkGlError("glCreateProgram");
            goto exit;
        }
        glAttachShader(program, vtxShader);
        glAttachShader(program, fragShader);

        glLinkProgram(program);
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (!linked) {
            ALOGE("Could not link program");
            GLint infoLogLen = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);
            if (infoLogLen) {
                GLchar* infoLog = (GLchar*)malloc(infoLogLen);
                if (infoLog) {
                    glGetProgramInfoLog(program, infoLogLen, NULL, infoLog);
                    ALOGE("Could not link program:\n%s\n", infoLog);
                    free(infoLog);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }

        exit:
        glDeleteShader(vtxShader);
        glDeleteShader(fragShader);
        return program;
    }
};
