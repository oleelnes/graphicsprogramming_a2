#include "MapLoader.h"

/**************************************************************
	This class loads the elements of the gamemap:
		- Loads the the level0.txt-file into an array
		- Loads all the textures into their respective variables
		- Loads the 3D object and returns the created VAO
		- Holds the wall, roof, floor and pellet arrays 
		- Loads the wall, roof, floor and pellet transformation vectors to be used to draw all of them
**************************************************************/

struct Vertex {
	glm::vec3 location;
	glm::vec3 normals;
	glm::vec2 texCoords;
};

MapLoader::MapLoader(){
	glGenVertexArrays(1, &VAO);
	levelShader = new Shader("resources/shaders/triangle.vs", "resources/shaders/triangle.fs");
	//wallShader = new Shader("resources/shaders/triangle.vs", "resources/shaders/triangle.fs");
	loadLevel();
	pelletMaker();
	pelletTexture = load_opengl_texture("resources/textures/pellet.jpg");
	wallTexture = load_opengl_texture("resources/textures/woodenwall.jpg");
	floorTexture = load_opengl_texture("resources/textures/floor.jpg");
	roofTexture = load_opengl_texture("resources/textures/roof.jpg");
	ghostTexture = load_opengl_texture("resources/textures/ghost.jpg");
}

void MapLoader::createPellet() {
	
	unsigned int VBO;

	//VAO
	//glGenVertexArrays(1, &this->VAO);


	glGenBuffers(1, &VBO);

	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pellet), pellet, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//normal attrib
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//texture attribs
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	
}

void MapLoader::createWall(){
	unsigned int VBO;

	glGenBuffers(1, &VBO);

	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(walll), walll, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//normal attrib
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//textire attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MapLoader::createFloorAndWall(){
	unsigned int VBO;

	glGenBuffers(1, &VBO);

	glBindVertexArray(this->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallFloorVertices), wallFloorVertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//normal attrib
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/*
	Heavily inspired lab05 -- altered such that it works for our program
*/
GLuint MapLoader::modelLoader(int& size)
{
	//Some variables that we are going to use to store data from tinyObj
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials; //This one goes unused for now, seeing as we don't need materials for this model.

	//Some variables incase there is something wrong with our obj file
	std::string warn;
	std::string err;

	//We use tinobj to load our models. Feel free to find other .obj files and see if you can load them.
	tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "resources/objects/Ball.obj", "resources/objects");
	
	if (!warn.empty()) {
		std::cout << warn << std::endl;
	}

	if (!err.empty()) {
		std::cerr << err << std::endl;
	}
	
	std::vector<Vertex> out_vertices{};
	std::vector<unsigned> out_indices{};
	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{

			out_vertices.push_back({});
			out_indices.push_back(out_indices.size());

			auto& vertex = out_vertices.back();

			vertex.location = {attrib.vertices[3 * index.vertex_index + 0],
								   attrib.vertices[3 * index.vertex_index + 1],
								   attrib.vertices[3 * index.vertex_index + 2] };

			vertex.normals = { attrib.normals[3 * index.normal_index + 0],
								  attrib.normals[3 * index.normal_index + 1],
								  attrib.normals[3 * index.normal_index + 2] };

			vertex.texCoords = { attrib.texcoords[2 * index.texcoord_index + 0],
									attrib.texcoords[2 * index.texcoord_index + 1] };
			
		}
	}



	GLuint tempVAO;
	glGenVertexArrays(1, &tempVAO);
	glBindVertexArray(tempVAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * out_vertices.size(), out_vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, location));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normals));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

	size = sizeof(out_vertices);

	std::cout << "vertices size: " << out_vertices.size() << endl;

	return tempVAO;
}

 
glm::vec3 MapLoader::getPelletVectors(int i)
{
	glm::vec3 tempname = allPellets[i];
	return tempname;
}

MapLoader::~MapLoader() noexcept
{
	//delete& allPellets;
}

void MapLoader::pelletMaker(){
	int pelletCount = 0;
	int floorRoofCount = 0;
	int wallCount = 0;
	for (int j = 0; j < 36; j++) {
		for (int i = 0; i < 28; i++) {
			int tempPos = i + (28 * j) + 1;
			//if either 0 is found, add to wall, pellet and roof
			//if 2 is found, add wall and floor, but not pellet
			//if 2 found, add playerposition TODO - not critical
			if (level[tempPos] == 0 || level[tempPos] == 2) {
				float zPos = 36.0f - j; //forwards
				float xPos = 0.0f + i; //to the right
				if (level[tempPos] != 2) {
					allPellets[pelletCount] = glm::vec3(xPos, -2.4f, zPos);
					pelletCount++;
				}
				pelletVisible[pelletCount] = true;
				allFloorAndRoofTiles[pelletCount] = glm::vec3(xPos, -4.0f, zPos);
				floorRoofCount++;
			}
			//if 1 is found in level-array, calculate the positions of that wall
			if (level[tempPos] == 1) {
				float zPos = 36.0f - j; //forwards
				float xPos = 0.0f + i; //to the right
				allWalls[wallCount] = glm::vec3(xPos, -3.0f, zPos);
				wallCount++;
			}
		}
	}
}

void MapLoader::loadLevel() {
	std::ifstream levelFile;
	int wallCounter = 0;
	level[0] = 99; //first element in the array won't be used, setting it to 99
	int number;
	levelFile.open("resources/other/level0.txt");
	if (levelFile.is_open()) {
		for (int i = 1; i <= 1008; i++) {
			levelFile >> number;
			level[i] = number;
		}
	}
	levelFile.close();

	for (int i = 0; i <= 1008; i++) {
		if (level[i] == 1) {
			wallCounter++;
		}
	}
	std::cout << "Number of walls is: " << wallCounter << "\n";
}


GLuint MapLoader::load_opengl_texture(const std::string& filepath)
{
	int w, h, bpp;

	
	auto pixels = stbi_load(filepath.c_str(), &w, &h, &bpp, STBI_rgb_alpha);
	if (!pixels) cout << "error: image failed to load" << endl;
	else cout << "success! image loaded" << endl;
	GLuint tex;
	glGenTextures(1, &tex);
	//glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	
	if (pixels) stbi_image_free(pixels);

	return tex;
}

