#include <GL/glew.h>
#include "primitive.h"
#include "YAFScene.h"

unsigned long waterlinePrimitive::movTime = 0;



float board_ctrlPoints[4][3] = {
	{-14, 0.0, 14},
	{-14, 0.0, -14},
	{14, 0.0, 14},
	{14, 0.0, -14}
};

float board_side_ctrlPoints[4][3] = {
	{-14, -1, 14},
	{-14, 0.0, 14},
	{14, -1, 14},
	{14, 0.0, 14}
};

float board_top_textpoints[4][2] = {	{ 0.0, 0.1},
{ 0.0, 1.0}, 
{ 0.5, 0.1},
{ 0.5, 1.0} };

float board_bottom_textpoints[4][2] = {	{ 0.5, 0.1},
{ 0.5, 1.0}, 
{ 1.0, 0.1},
{ 1.0, 1.0} };

float board_side_textpoints[4][2] = {	{ 0.0, 0.0},
{ 0.0, 0.1}, 
{ 1.0, 0.0},
{ 1.0, 0.1} };

//---------Plane points-----------------

float plane_ctrlpoints[4][3] = {	
	{  -0.5, 0.0, 0.5},
	{ -0.5, 0.0, -0.5},
	{  0.5, 0.0, 0.5}, 
	{ 0.5, 0.0, -0.5} 
};

float plane_nrmlcompon[4][3] = {	{  0.0, 1.0, 0.0},
{  0.0, 1.0, 0.0}, 
{  0.0, 1.0, 0.0},
{  0.0, 1.0, 0.0} };

float plane_textpoints[4][2] = {	{ 0.0, 0.0},
{ 0.0, 1.0}, 
{ 1.0, 0.0},
{ 1.0, 1.0} };

//------------Boat points------------------

float sail_ctrlPoints[6][3] = {
	{ -1, 0.6, 0.1},
	{ -1, 1.6, 0.4},
	{ -1, 2.6, 0.1},
	{ 1, 0.6, 0.1},
	{ 1, 1.6, 0.4},
	{ 1, 2.6, 0.1}
};

float sail_textPoints[4][2]  = {
	{0.0, 0.0},
	{0.0, 0.5},
	{0.25, 0.0},
	{0.25, 0.5}
};

float side_textPoints[4][2] = {
	{0.0, 0.5},
	{0.0, 1.0},
	{1.0, 0.5},
	{1.0, 1.0}
};

float deck_textPoints[4][2]  = {
	{0.25, 0.0},
	{0.25, 0.5},
	{1.0, 0.0},
	{1.0, 0.5}
};

float side_ctrlPoints[9][3] = {
	{ -0.7, 0, -1.5},
	{ -0.7, 0.25, -1.5},
	{ -0.7, 0.5, -1.5},
	{ -0.7, 0, 0.5},
	{ -0.7, 0.25, 0.5},
	{ -0.7, 0.5, 0.5},
	{ 0, 0, 1.3},
	{ 0, 0.25, 1.4},
	{ 0, 0.5, 1.5}
};

float deck_ctrlPoints[9][3] = {
	{ -0.7, 0.5, -1.5},
	{ 0, 0.5, -1.5},
	{ 0.7, 0.5, -1.5},
	{ -0.7, 0.5, 0.5},
	{ 0, 0.5, 0.5},
	{ 0.7, 0.5, 0.5},
	{ 0, 0.5, 1.5},
	{ 0, 0.5, 1.5},
	{ 0, 0.5, 1.5}
};

float back_ctrlPoints[4][3] = {
	{0.7, 0, -1.5},
	{0.7, 0.5, -1.5},
	{-0.7, 0, -1.5},
	{-0.7, 0.5, -1.5}
};

//----------------Waterline points-----------------

float waterline_ctrlpoints[4][3] = {	{  -2.5, 0.0, 17.5},
{ -2.5, 0.0, -2.5},
{  2.5, 0.0, 17.5}, 
{ 2.5, 0.0, -2.5} };

float mast_colorVals[4] = {0.35f, 0.18f,0.09f,1};
float mast_emissiveness[4] = {0,0,0,0};
float mast_shininess = 120;

float selected_colorVals[4] = {0.6,0.8,1,0.5};
float selected_emissiveness[4] = {0,0,0,0};
float selected_shininess = 120;


GLUquadric* primitive::primitivesQuadric = gluNewQuadric();


void modelPrimitive::draw(){
	glBegin(GL_TRIANGLES);
	for(unsigned int i=0; i < vertices.size();i++ ){
		glNormal3f(normals[i].x,normals[i].y,normals[i].z);
		//glTexCoord2f(uvs[i].x,uvs[i].y);
		glVertex3f(vertices[i].x,vertices[i].y,vertices[i].z);

	}
	glEnd();

}
modelPrimitive::modelPrimitive(const char* path){
	std::vector< unsigned int > vertexIndices, normalIndices;
	std::vector< glm::vec3 > temp_vertices;
	std::vector< glm::vec3 > temp_normals;
	char* filepath = (char*) malloc(strlen(path)+strlen("resources/")+1);
	strcpy(filepath, "resources/");
	strcat(filepath, path);
	FILE * file = fopen(filepath, "r");
	free(filepath);
	if( file == NULL ){
		printf("Impossible to open the file !\n");
		throw unableToLoadObject(path);
	}

	while( 1 ){

		char lineHeader[128];
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		// else : parse lineHeader
		if ( strcmp( lineHeader, "v" ) == 0 ){
			glm::vec3 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
			temp_vertices.push_back(vertex);
		}else if ( strcmp( lineHeader, "vn" ) == 0 ){
			glm::vec3 normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
			temp_normals.push_back(normal);
		}else if ( strcmp( lineHeader, "f" ) == 0 ){
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2] );
			if (matches != 6){
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				throw unableToLoadObject(path);
			}
			vertexIndices.push_back(vertexIndex[0]);
			vertexIndices.push_back(vertexIndex[1]);
			vertexIndices.push_back(vertexIndex[2]);
			normalIndices.push_back(normalIndex[0]);
			normalIndices.push_back(normalIndex[1]);
			normalIndices.push_back(normalIndex[2]);
		}
	}
	for( unsigned int i=0; i<vertexIndices.size(); i++ ){
		unsigned int vertexIndex = vertexIndices[i];
		glm::vec3 vertex = temp_vertices[ vertexIndex-1 ];
		vertices.push_back(vertex);
	}
	for( unsigned int i=0; i<vertexIndices.size(); i++ ){
		unsigned int vertexIndex = normalIndices[i];
		glm::vec3 vertex = temp_normals[ vertexIndex-1 ];
		normals.push_back(vertex);
	}
	fclose(file);
}


vehiclePrimitive::vehiclePrimitive(GLenum faceCullOrder, myAppearance* parentAppearance) {

	this->faceCullOrder = faceCullOrder;

	this->mast = cylinderPrimitive(0.05,0.05,2.3,10,10);
	this->backSide = rectanglePrimitive(-0.7,0,0.7,0.5);

	this->mastAppearance = new myAppearance(mast_emissiveness,mast_colorVals, mast_colorVals,mast_colorVals, mast_shininess);
	this->parentAppearance = parentAppearance;
}

void vehiclePrimitive::draw(){
	//----------------SAIL FRONT------------------------
	glFrontFace(GL_CCW);
	glEnable(GL_AUTO_NORMAL);
	glMap2f(GL_MAP2_TEXTURE_COORD_2,  0.0, 1, 2, 2,  0.0, 1, 4, 2,  &sail_textPoints[0][0]);
	glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 3,  0.0, 1.0, 9, 2,  &sail_ctrlPoints[0][0]);

	glMapGrid2f(10, 0.0,1.0, 10, 0.0,1.0); 

	glEvalMesh2(GL_FILL, 0,10, 0,10);

	//---------------------SAIL BACK---------------------
	glFrontFace(GL_CW);
	glEnable(GL_AUTO_NORMAL);
	glMap2f(GL_MAP2_TEXTURE_COORD_2,  0.0, 1, 2, 2,  0.0, 1, 4, 2,  &sail_textPoints[0][0]);
	glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 3,  0.0, 1.0, 9, 2,  &sail_ctrlPoints[0][0]);

	glMapGrid2f(10, 0.0,1.0, 10, 0.0,1.0); 

	glEvalMesh2(GL_FILL, 0,10, 0,10);

	//-------------------SIDE LEFT--------------------
	glFrontFace(GL_CCW);
	glMap2f(GL_MAP2_TEXTURE_COORD_2,  0.0, 1, 2, 2,  0.0, 1, 4, 2,  &side_textPoints[0][0]);
	glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 3,  0.0, 1.0, 9, 3,  &side_ctrlPoints[0][0]);

	glMapGrid2f(10, 0.0,1.0, 10, 0.0,1.0); 

	glEvalMesh2(GL_FILL, 0,10, 0,10);

	//----------------------SIDE RIGHT--------------------

	glPushMatrix();
	glScaled(-1,1,1);
	glFrontFace(GL_CW);
	glMap2f(GL_MAP2_TEXTURE_COORD_2,  0.0, 1, 2, 2,  0.0, 1, 4, 2,  &deck_textPoints[0][0]);
	glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 3,  0.0, 1.0, 9, 3,  &side_ctrlPoints[0][0]);

	glMapGrid2f(10, 0.0,1.0, 10, 0.0,1.0); 

	glEvalMesh2(GL_FILL, 0,10, 0,10);
	glPopMatrix();

	//--------------------DECK-----------------------

	glFrontFace(GL_CCW);
	glMap2f(GL_MAP2_TEXTURE_COORD_2,  0.0, 1, 2, 2,  0.0, 1, 4, 2,  &deck_textPoints[0][0]);
	glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 3,  0.0, 1.0, 9, 3,  &deck_ctrlPoints[0][0]);

	glMapGrid2f(10, 0.0,1.0, 10, 0.0,1.0); 

	glEvalMesh2(GL_FILL, 0,10, 0,10);

	//---------------------DECK BELLOW--------------------------------

	glPushMatrix();
	glTranslated(0,-0.5,0);
	glFrontFace(GL_CW);
	glMap2f(GL_MAP2_TEXTURE_COORD_2,  0.0, 1, 2, 2,  0.0, 1, 4, 2,  &deck_textPoints[0][0]);
	glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 3,  0.0, 1.0, 9, 3,  &deck_ctrlPoints[0][0]);

	glMapGrid2f(10, 0.0,1.0, 10, 0.0,1.0); 

	glEvalMesh2(GL_FILL, 0,10, 0,10);
	glPopMatrix();

	//-----------------------BACK-----------------------------------

	glFrontFace(GL_CCW);
	glMap2f(GL_MAP2_TEXTURE_COORD_2,  0.0, 1, 2, 2,  0.0, 1, 4, 2,  &deck_textPoints[0][0]);
	glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 2,  0.0, 1.0, 6, 2,  &back_ctrlPoints[0][0]);

	glMapGrid2f(10, 0.0,1.0, 10, 0.0,1.0); 

	glEvalMesh2(GL_FILL, 0,10, 0,10);

	glFrontFace(faceCullOrder);
	glDisable(GL_AUTO_NORMAL);
	//---------------------MAST--------------------------------

	mastAppearance->apply();
	glPushMatrix();
	glTranslated(0,0.5,0);
	glRotated(-90,1,0,0);
	mast.draw();
	glPopMatrix();
	parentAppearance->apply();
}

patchPrimitive::patchPrimitive(vector<myPoint> &patchControlPoints, int partsU, int partsV, int computeType, int order, GLenum faceCullOrder){
	for(unsigned int i=0; i < patchControlPoints.size(); i++){
		ctrlPoints[i][0] = patchControlPoints[i].x;
		ctrlPoints[i][1] = patchControlPoints[i].y;
		ctrlPoints[i][2] = patchControlPoints[i].z;
	}
	this->partsU=partsU;
	this->partsV=partsV;
	this->order=order;
	this->faceCullOrder = faceCullOrder;

	if(computeType == nodeChild::POINT)
		this->computingType = GL_POINT;
	else if(computeType == nodeChild::LINE)
		this->computingType = GL_LINE;
	else
		this->computingType = GL_FILL;
}
void patchPrimitive::draw(){
	glFrontFace(GL_CW);
	glEnable(GL_AUTO_NORMAL);
	glMap2f(GL_MAP2_TEXTURE_COORD_2,  0.0, 1, 2, 2,  0.0, 1, 4, 2,  &plane_textpoints[0][0]);
	glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, order+1,  0.0, 1.0, 3*(order+1), order+1,  &ctrlPoints[0][0]);

	glMapGrid2f(partsU, 0.0,1.0, partsV, 0.0,1.0); 

	glEvalMesh2(computingType, 0,partsU, 0,partsV);

	glFrontFace(faceCullOrder);
	glDisable(GL_AUTO_NORMAL);
}

planePrimitive::planePrimitive(int parts, float s_len, float t_len) : parts(parts) {
	s_max=s_len;
	t_max=t_len;
}

void planePrimitive::draw(){

	glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 2,  0.0, 1.0, 6, 2,  &plane_ctrlpoints[0][0]);
	glMap2f(GL_MAP2_NORMAL,   0.0, 1.0, 3, 2,  0.0, 1.0, 6, 2,  &plane_nrmlcompon[0][0]);
	glMap2f(GL_MAP2_TEXTURE_COORD_2,  0.0, s_max, 2, 2,  0.0, t_max, 4, 2,  &plane_textpoints[0][0]);

	glMapGrid2f(parts, 0.0,1.0, parts, 0.0,1.0); 


	glEvalMesh2(GL_FILL, 0,parts, 0,parts);

}

rectanglePrimitive::rectanglePrimitive(float x1, float y1, float x2, float y2)
{
	this->x1 = x1;
	this->x2 = x2;
	this->y1 = y1;
	this->y2 = y2;
	float sizeS = abs(x2-x1);
	float sizeT = abs(y2-y1);
	if(sizeS > 10)
		subdivisions_s = 50;
	else if(sizeS > 1)
		subdivisions_s = 20;
	else
		subdivisions_s = 5;
	if(sizeT > 10)
		subdivisions_t = 50;
	else if(sizeT > 1)
		subdivisions_t = 20;
	else
		subdivisions_t = 5;

	side_x = (x2-x1)/subdivisions_s;
	side_y = (y2-y1)/subdivisions_t;

}

rectanglePrimitive::rectanglePrimitive(float x1, float y1, float x2, float y2, float texlength_s, float texlength_t)
{
	this->x1 = x1;
	this->x2 = x2;
	this->y1 = y1;
	this->y2 = y2;
	float sizeS = abs(x2-x1);
	float sizeT = abs(y2-y1);
	this->s_x2 = sizeS/texlength_s;
	this->t_y2 = sizeT/texlength_t;
	if(sizeS > 10)
		subdivisions_s = 50;
	else if(sizeS > 1)
		subdivisions_s = 20;
	else
		subdivisions_s = 5;
	if(sizeT > 10)
		subdivisions_t = 50;
	else if(sizeT > 1)
		subdivisions_t = 20;
	else
		subdivisions_t = 5;
	side_x = (x2-x1)/subdivisions_s;
	side_y = (y2-y1)/subdivisions_t;
	side_s = s_x2/subdivisions_s;
	side_t = t_y2/subdivisions_t;
}

void rectanglePrimitive::draw()
{
	float iniS = 0;
	float iniX = x1;
	for(int i=0; i < subdivisions_s; i++){
		glBegin(GL_TRIANGLE_STRIP);
		float iniT = 0;
		float iniY = y1;
		glTexCoord2d(iniS,0);

		glNormal3f(0,0,1);
		glVertex3f(iniX, iniY, 0);
		for(int j=0; j < subdivisions_t; j++){
			glTexCoord2d(iniS+side_s,iniT);
			glNormal3f(0,0,1);
			glVertex3f(iniX+side_x, iniY, 0);

			glTexCoord2d(iniS,iniT+side_t);
			glNormal3f(0,0,1);
			glVertex3f(iniX, iniY+side_y, 0);
			iniY += side_y;
			iniT += side_t;
		}
		glTexCoord2d(iniS+side_s,iniT);
		glNormal3f(0,0,1);
		glVertex3f(iniX+side_x, iniY, 0);
		iniX += side_x;
		iniS += side_s;
		glEnd( );	
	}
}

trianglePrimitive::trianglePrimitive(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{
	this->x1 = x1;this->x2 = x2;this->x3 = x3;
	this->y1 = y1;this->y2 = y2;this->y3 = y3;
	this->z1 = z1;this->z2 = z2;this->z3 = z3;

	myPoint p1(x1,y1,z1), p2(x2,y2,z2), p3(x3,y3,z3);
	vector<myPoint> pointList; pointList.push_back(p1);pointList.push_back(p2);pointList.push_back(p3);
	myPoint surfaceNormal = newell_method(pointList);
	normalX = surfaceNormal.x;
	normalY = surfaceNormal.y;
	normalZ = surfaceNormal.z;
}

trianglePrimitive::trianglePrimitive(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float texlength_s, float texlength_t){
	this->x1 = x1;this->x2 = x2;this->x3 = x3;
	this->y1 = y1;this->y2 = y2;this->y3 = y3;
	this->z1 = z1;this->z2 = z2;this->z3 = z3;

	myPoint p1(x1,y1,z1), p2(x2,y2,z2), p3(x3,y3,z3);
	vector<myPoint> pointList; pointList.push_back(p1);pointList.push_back(p2);pointList.push_back(p3);
	myPoint surfaceNormal = newell_method(pointList);
	normalX = surfaceNormal.x;
	normalY = surfaceNormal.y;
	normalZ = surfaceNormal.z;


	float p1_p2 = sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y)+(p1.z-p2.z)*(p1.z-p2.z));
	float p1_p3 = sqrt((p1.x-p3.x)*(p1.x-p3.x)+(p1.y-p3.y)*(p1.y-p3.y)+(p1.z-p3.z)*(p1.z-p3.z));
	float p3_p2 = sqrt((p3.x-p2.x)*(p3.x-p2.x)+(p3.y-p2.y)*(p3.y-p2.y)+(p3.z-p2.z)*(p3.z-p2.z));
	float semi_perimeter = (p1_p2+p1_p3+p3_p2)/2;
	float areaTriangle = sqrt(semi_perimeter*(semi_perimeter-p1_p2)*(semi_perimeter-p1_p3)*(semi_perimeter-p3_p2));

	//A = b*h/2. sizeT = h. sizeS = b.
	float sizeS = p1_p2;
	float sizeT = areaTriangle*2/sizeS;

	s_max = sizeS/texlength_s;
	t_max = sizeT/texlength_t;

	float s_p3_to_p1 = sqrt(p1_p3*p1_p3 - sizeT*sizeT);
	s_p3 = s_p3_to_p1/sizeS * s_max;

	float s_p3_to_p2 = sqrt(p3_p2*p3_p2 - sizeT*sizeT);
	if(s_p3_to_p2 > p1_p2 && s_p3_to_p2 > s_p3_to_p1) //case where P3 is 'behind' P1
		s_p3 = -s_p3;

}


void trianglePrimitive::draw()
{
	glBegin(GL_POLYGON);
	glNormal3f(normalX,normalY,normalZ);
	glTexCoord2d(0,0);
	glVertex3f(x1,y1,z1);
	glTexCoord2d(s_max,0);
	glVertex3f(x2,y2,z2);
	glTexCoord2d(s_p3, t_max);
	glVertex3f(x3,y3,z3);
	glEnd();
}

spherePrimitive::spherePrimitive(float radius, int slices, int stacks)
{
	this->radius = radius;
	this->slices = slices;
	this->stacks = stacks;
}

void spherePrimitive::draw()
{
	gluSphere(primitivesQuadric, radius,slices,stacks);
}

cylinderPrimitive::cylinderPrimitive(float bRadius, float tRadius, float height, int slices, int stacks)
{

	this->baseRadius = bRadius;
	this->topRadius = tRadius;
	this->slices = slices;
	this->stacks = stacks;
	this->height = height;
}

void cylinderPrimitive::draw()
{
	gluCylinder(primitivesQuadric,baseRadius,topRadius,height,slices,stacks);

	glPushMatrix();
	glRotated(180,0,1,0);
	gluDisk(primitivesQuadric,0,baseRadius,slices,1);
	glPopMatrix();

	glPushMatrix();
	glTranslated(0,0,height);
	gluDisk(primitivesQuadric,0,topRadius,slices,1);
	glPopMatrix();
}

torusPrimitive::torusPrimitive(float inner, float outer, int slices, int loops)
{
	this->inner = inner;
	this->outer = outer;
	this->loops = loops;
	this->slices = slices;
}

void torusPrimitive::draw()
{

	int i, j;
	GLfloat theta, phi, theta1;
	GLfloat cosTheta, sinTheta;
	GLfloat cosTheta1, sinTheta1;
	GLfloat ringDelta, sideDelta;

	ringDelta = 2.0 * M_PI / loops;
	sideDelta = 2.0 * M_PI / slices;


	theta = 0.0;
	cosTheta = 1.0;
	sinTheta = 0.0;
	for (i = loops - 1; i >= 0; i--) {
		theta1 = theta + ringDelta;
		cosTheta1 = cos(theta1);
		sinTheta1 = sin(theta1);
		phi = 0.0;
		glBegin(GL_QUAD_STRIP);
		for (j = slices; j >= 0; j--) {
			GLfloat cosPhi, sinPhi, dist;

			phi += sideDelta;
			cosPhi = cos(phi);
			sinPhi = sin(phi);
			dist = outer + inner * cosPhi;

			glTexCoord2d(cosTheta1 * dist, -sinTheta1 * dist);
			glNormal3f(cosTheta1 * cosPhi, -sinTheta1 * cosPhi, sinPhi);
			glVertex3f(cosTheta1 * dist, -sinTheta1 * dist, inner * sinPhi);

			glTexCoord2d(cosTheta * dist, -sinTheta * dist);
			glNormal3f(cosTheta * cosPhi, -sinTheta * cosPhi, sinPhi);
			glVertex3f(cosTheta * dist, -sinTheta * dist,  inner * sinPhi);
		}
		theta = theta1;
		cosTheta = cosTheta1;
		sinTheta = sinTheta1;
		glEnd();
	}

}

waterlinePrimitive::waterlinePrimitive(string vertexShader, string fragmentShader, string heightmap, string texture)
{
	init(("resources/" + vertexShader).c_str(), ("resources/" + fragmentShader).c_str());

	CGFshader::bind();

	baseTexture = new CGFtexture("resources/" + texture);
	secTexture = new CGFtexture("resources/" + heightmap);

	movTimeLoc = glGetUniformLocation(id(), "movTime");

	baseImageLoc = glGetUniformLocation(id(), "baseImage");
	glUniform1i(baseImageLoc, 0);

	secImageLoc = glGetUniformLocation(id(), "secondImage");
	glUniform1i(secImageLoc, 1);
}

void waterlinePrimitive::draw() {
	if(YAFScene::drawHUD){
		this->bind();
		glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 2,  0.0, 1.0, 6, 2,  &waterline_ctrlpoints[0][0]);
		glMap2f(GL_MAP2_NORMAL,   0.0, 1.0, 3, 2,  0.0, 1.0, 6, 2,  &plane_nrmlcompon[0][0]);
		glMap2f(GL_MAP2_TEXTURE_COORD_2,  0.0, 0.2, 2, 2,  0.0, 1, 4, 2,  &plane_textpoints[0][0]);

		glMapGrid2f(100, 0.0,1.0, 20, 0.0,1.0); 


		glEvalMesh2(GL_FILL, 0,100, 0,20);
		this->unbind();
	}
}

void waterlinePrimitive::bind(void)
{
	CGFshader::bind();

	float moveScale = ((float)movTime) / 5000.0;
	glUniform1f(movTimeLoc, moveScale);

	glActiveTexture(GL_TEXTURE0);
	baseTexture->apply();

	glActiveTexture(GL_TEXTURE1);
	secTexture->apply();

	glActiveTexture(GL_TEXTURE0);
}

boardPrimitive::boardPrimitive(GLenum faceCullOrder) : faceCullOrder(faceCullOrder)
{
	this->selectedAppearance = new myAppearance(selected_emissiveness,selected_colorVals, selected_colorVals,selected_colorVals, selected_shininess);
	resetSelection();
}

void boardPrimitive::draw()
{
	glFrontFace(GL_CCW);
	glEnable(GL_AUTO_NORMAL);
	glMap2f(GL_MAP2_TEXTURE_COORD_2,  0.0, 1, 2, 2,  0.0, 1, 4, 2,  &board_top_textpoints[0][0]);
	glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 2,  0.0, 1.0, 6, 2,  &board_ctrlPoints[0][0]);
	glMapGrid2f(10, 0.0,1.0, 10, 0.0,1.0); 
	glEvalMesh2(GL_FILL, 0,10, 0,10);

	glPushMatrix();
	glFrontFace(GL_CW);
	glTranslated(0,-1,0);
	glMap2f(GL_MAP2_TEXTURE_COORD_2,  0.0, 1, 2, 2,  0.0, 1, 4, 2,  &board_bottom_textpoints[0][0]);
	glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 2,  0.0, 1.0, 6, 2,  &board_ctrlPoints[0][0]);
	glMapGrid2f(10, 0.0,1.0, 10, 0.0,1.0); 
	glEvalMesh2(GL_FILL, 0,10, 0,10);
	glPopMatrix();

	glFrontFace(GL_CCW);
	for(unsigned int i = 0; i < 4; i++)
	{
		glRotated(i*90,0,1,0);
		glMap2f(GL_MAP2_TEXTURE_COORD_2,  0.0, 1, 2, 2,  0.0, 1, 4, 2,  &board_side_textpoints[0][0]);
		glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, 2,  0.0, 1.0, 6, 2,  &board_side_ctrlPoints[0][0]);
		glMapGrid2f(10, 0.0,1.0, 10, 0.0,1.0); 
		glEvalMesh2(GL_FILL, 0,10, 0,10);
	}

	glFrontFace(faceCullOrder);

	glPushName(-1);		


	for(unsigned int i = 0; i < 5; i++)
	{
		glPushMatrix();
		glTranslatef(i*5-12.5, 0, 0);
		glLoadName(4-i);

		for(unsigned int j = 0; j < 5; j++)
		{
			if(!(selectedX1 == 4-i && selectedY1 == j) && !(selectedX2 == 4-i && selectedY2 == j))
			{
				glColorMask(false, false, false, false);
			}
			else
			{
				selectedAppearance->apply();
				glEnable (GL_BLEND); 
				glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			glPushMatrix();
			glTranslatef(0,0.01,j*5-12.5);
			glPushName(j);

			glBegin(GL_POLYGON);
			glVertex3f(0,0,0);
			glNormal3f(0,1,0);
			glVertex3f(0,0,5);
			glNormal3f(0,1,0);
			glVertex3f(5,0,5);
			glNormal3f(0,1,0);
			glVertex3f(5,0,0);
			glNormal3f(0,1,0);
			glEnd();

			glPopName();
			glPopMatrix();
			if(!(selectedX1 == 4-i && selectedY1 == j) && !(selectedX2 == 4-i && selectedY2 == j))
				glColorMask(true, true, true, true);
			else
			{
				glDisable (GL_BLEND); 
			}
		}
		glPopMatrix();
	}
}

void boardPrimitive::setSelected( int x, int y )
{
	if(selectedX2 == -1 && selectedY2 == -1)
	{
		selectedX2 = x;
		selectedY2 = y;
	}
	else
	{
		selectedX1 = x;
		selectedY1 = y;
	}
}

void boardPrimitive::resetSelection()
{
	selectedX1 = -1;
	selectedX2 = -1;
	selectedY1 = -1;
	selectedY2 = -1;
}
