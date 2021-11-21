#version 430 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D uTexture; //gets texture input from ACTIVETEXTURE0 

uniform vec3 viewPos;

uniform vec3 lightPosition;  
uniform vec3 lightDirection;
uniform float lightCutOff;
uniform float lightOuterCutOff;
  
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;
	
uniform float lightConstant;
uniform float lightLinear;
uniform float lightQuadratic;
uniform vec4 u_color = vec4(1.0, 1.0, 1.0, 1.0);

uniform float materialShininess;



void main(){
	// ambient
    vec3 ambient = lightAmbient * texture(uTexture, TexCoords).rgb;
    
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPosition - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = lightDiffuse * diff * texture(uTexture, TexCoords).rgb;  
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), materialShininess);
    vec3 specular = lightSpecular * spec * texture(uTexture, TexCoords).rgb;  
    
    // spotlight (soft edges)
    float theta = dot(lightDir, normalize(-lightDirection)); 
    float epsilon = (lightCutOff - lightOuterCutOff);
    float intensity = clamp((theta - lightOuterCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;
    
    // attenuation
    float distance    = length(lightPosition - FragPos);
    float attenuation = 1.0 / (lightConstant + lightLinear * distance + lightQuadratic * (distance * distance));    
    ambient  *= attenuation; 
    diffuse   *= attenuation;
    specular *= attenuation;   
        
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0) * u_color;
}