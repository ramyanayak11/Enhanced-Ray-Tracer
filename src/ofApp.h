// Ramya Nayak
// 
// Final Project - Enhanced Ray Tracer
// CS 116A - Introduction to Computer Graphics
//
// December 13, 2024



// Acknowledgements: 
//		Some starter code was provided by Professor Kevin Smith during past projects.



#pragma once

#include "ofMain.h"
#include "ofxGUI.h"
#include "ofxAssimpModelLoader.h"
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/euler_angles.hpp>
using namespace std;


//  General Purpose Ray class
//
class Ray {
public:
	Ray(glm::vec3 p, glm::vec3 d) { this->p = p; this->d = d; }
	void draw(float t) { ofDrawLine(p, p + t * d); }

	glm::vec3 evalPoint(float t) {
		return (p + t * d);
	}

	glm::vec3 p, d;
};

//  Base class for any renderable object in the scene
//
class SceneObject {
public:
	virtual ~SceneObject() = default;
	virtual void draw() = 0;  
	virtual bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal) { return false; }
	virtual glm::vec3 getNormal(const glm::vec3& p) { return glm::vec3(1, 0, 0); }

	virtual ofColor textureLookup(ofImage* t, glm::vec3 intersection) { return diffuseColor; } 

	virtual glm::vec3 bumpLookup(ofImage *t, glm::vec3 intersection, glm::vec3 originalNormal) { return  glm::vec3(1, 0, 0); }
	virtual void changeTexture(string tName, string t = "", string s = "") { return; } 

	glm::mat4 getMatrix() {
		glm::mat4 T = glm::translate(glm::mat4(1.0), glm::vec3(position));
		glm::mat4 R = glm::eulerAngleYXZ(glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z));   // yaw, pitch, roll
		glm::mat4 S = glm::scale(glm::mat4(1.0), scale);      // added this scale if you want to change size of object
		return T * R * S; // correct order: T, R, S		global origin rotation order: R, T, S
	}
	void setPosition(glm::vec3 p) {
		position = p;
		transform = getMatrix();
	}
	void resetTransform() {
		transform = getMatrix();
	}

	virtual string getType() {
		return "obj";
	}

	// material properties
	//
	ofColor diffuseColor = ofColor::grey;    // default colors - can be changed.
	ofColor specularColor = ofColor::lightGray;


	// any data common to all scene objects goes here
	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 rotation = glm::vec3(0, 0, 0);    // degrees
	glm::vec3 scale = glm::vec3(1, 1, 1);
	glm::mat4 transform;

	// selection
	//
	bool isSelectable = true;
	bool isSelected = false;
	string name = "SceneObject";

	ofImage texture;
	ofImage specular;
	ofImage bump;
	string textureName;
};


//  General purpose plane
//
class Plane : public SceneObject {
public:
	Plane(glm::vec3 p, glm::vec3 n, ofColor diffuse = ofColor::green, string tName = "", string t = "", string s = "", string b = "", float tf = 10.0f, float w = 100, float h = 100) {
		position = p; normal = n;
		width = w;
		height = h;
		diffuseColor = diffuse;
		tilingFactor = tf;
		textureName = tName;

		if (t != "") {
			texture.load(t);
			if (texture.isAllocated()) {
				cout << "plane texture loaded" << endl;
				cout << "\theight: " << texture.getHeight() << endl;
				cout << "\twidth: " << texture.getWidth() << endl;
			}
			else {
				cout << "plane texture failed to load" << endl;
			}
		}
		if (s != "") {
			specular.load(s);
			if (specular.isAllocated()) {
				cout << "plane specular loaded" << endl;
				cout << "\theight: " << texture.getHeight() << endl;
				cout << "\twidth: " << texture.getWidth() << endl;
			}
			else {
				cout << "plane specular failed to load" << endl;
			}
		}
		if (b != "") {
			bump.load(b);
			if (bump.isAllocated()) {
				cout << "plane bump loaded" << endl;
				cout << "\theight: " << bump.getHeight() << endl;
				cout << "\twidth: " << bump.getWidth() << endl;
			}
			else {
				cout << "plane bump failed to load" << endl;
			}
		}

		if (normal == glm::vec3(0, 1, 0))
			plane.rotateDeg(-90, 1, 0, 0);
		else if (normal == glm::vec3(0, -1, 0))
			plane.rotateDeg(90, 1, 0, 0);
		else if (normal == glm::vec3(1, 0, 0))
			plane.rotateDeg(90, 0, 1, 0);
		else if (normal == glm::vec3(-1, 0, 0))
			plane.rotateDeg(-90, 0, 1, 0);
	}
	Plane() {
		normal = glm::vec3(0, 1, 0);
		plane.rotateDeg(90, 1, 0, 0);
		isSelectable = false;
	}

	void changeTexture(string tName, string t = "", string s = "") override { 
		textureName = tName;
		if (t != "") {
			texture.load(t);
			if (texture.isAllocated()) {
				cout << "plane texture loaded" << endl;
				cout << "\theight: " << texture.getHeight() << endl;
				cout << "\twidth: " << texture.getWidth() << endl;
			}
			else {
				cout << "plane texture failed to load" << endl;
			}
		}
		if (s != "") {
			specular.load(s);
			if (specular.isAllocated()) {
				cout << "plane specular loaded" << endl;
				cout << "\theight: " << texture.getHeight() << endl;
				cout << "\twidth: " << texture.getWidth() << endl;
			}
			else {
				cout << "plane specular failed to load" << endl;
			}
		}
	} 

	bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal);
	glm::vec3 getNormal(const glm::vec3& p) { return this->normal; }
	void draw() {
		plane.setPosition(position);
		plane.setWidth(width);
		plane.setHeight(height);
		plane.setResolution(4, 4);
		plane.draw();
	}


	ofColor textureLookup(ofImage* t, glm::vec3 intersection) override;
	glm::vec3 bumpLookup(ofImage *t, glm::vec3 intersection, glm::vec3 originalNormal) override;

	string getType() override {
		return "plane";
	}

	ofPlanePrimitive plane;
	glm::vec3 normal;

	float width = 20;
	float height = 20;

	ofImage texture;
	ofImage specular;
	ofImage bump;

	float tilingFactor;

};


//  General purpose sphere  (assume parametric)
//
class Sphere : public SceneObject {
public:
	Sphere(glm::vec3 p, float r, ofColor diffuse = ofColor::lightGray, string tName = "", string t = "", string s = "", float tf = 10.0f) {
		position = p; radius = r; diffuseColor = diffuse;
		tilingFactor = tf;
		spherePrim.setResolution(100);
		textureName = tName;

		if (t != "") {
			texture.load(t);
			if (texture.isAllocated()) {
				cout << "sphere texture loaded" << endl;
				cout << "\theight: " << texture.getHeight() << endl;
				cout << "\twidth: " << texture.getWidth() << endl;
			}
			else {
				cout << "sphere texture failed to load" << endl;
			}
		}
		if (s != "") {
			specular.load(s);
			if (specular.isAllocated()) {
				cout << "sphere specular loaded" << endl;
				cout << "\theight: " << texture.getHeight() << endl;
				cout << "\twidth: " << texture.getWidth() << endl;
			}
			else {
				cout << "sphere specular failed to load" << endl;
			}
		}
	}
	Sphere() {}

	void changeTexture(string tName, string t = "", string s = "") override { 
		textureName = tName;

		if (t != "") {
			texture.load(t);
			if (texture.isAllocated()) {
				cout << "sphere texture loaded" << endl;
				cout << "\theight: " << texture.getHeight() << endl;
				cout << "\twidth: " << texture.getWidth() << endl;
			}
			else {
				cout << "sphere texture failed to load" << endl;
			}
		}
		if (s != "") {
			specular.load(s);
			if (specular.isAllocated()) {
				cout << "sphere specular loaded" << endl;
				cout << "\theight: " << texture.getHeight() << endl;
				cout << "\twidth: " << texture.getWidth() << endl;
			}
			else {
				cout << "sphere specular failed to load" << endl;
			}
		}
	}

	bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal) {
		// accounts for scaling
		glm::vec3 scaledRayOrigin = ray.p / scale;
		glm::vec3 scaledRayDirection = ray.d / scale;
		Ray scaledRay(scaledRayOrigin, glm::normalize(scaledRayDirection));

		bool hit = glm::intersectRaySphere(scaledRay.p, scaledRay.d, position, radius, point, normal);

		if (hit) {
			point = point * scale;						// converts intersection point and normal back to world space
			normal = glm::normalize(normal / scale);
		}

		return hit;
	}

	glm::vec3 getNormal(const glm::vec3& p) { return glm::normalize(p - position); }

	void draw() {	// new sphere draw method
		ofPushMatrix();
		ofMultMatrix(getMatrix());
		spherePrim.setRadius(radius);
		spherePrim.draw();
		ofPopMatrix();
	}

	string getType() override {
		return "sphere";
	}

	ofColor textureLookup(ofImage* t, glm::vec3 intersection) override;

	float radius = 1.0;
	ofSpherePrimitive spherePrim;

	ofImage texture;
	ofImage specular;
	float tilingFactor;

};



class TargetSphere : public SceneObject {
public:
	TargetSphere(glm::vec3 p, float r, ofColor diffuse = ofColor::lightGray) { position = p; radius = r; diffuseColor = diffuse; }
	TargetSphere() {}
	bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal) {
		return (glm::intersectRaySphere(ray.p, ray.d, position, radius, point, normal));
	}
	glm::vec3 getNormal(const glm::vec3& p) { return glm::normalize(p - position); }

	void draw() {	// new sphere draw method
		ofPushMatrix();
		ofMultMatrix(getMatrix());
		spherePrim.setRadius(radius);
		spherePrim.draw();
		ofPopMatrix();
	}

	string getType() override {
		return "targetSphere";
	}

	float radius = 1.0;
	ofSpherePrimitive spherePrim;

};

class Cone : public SceneObject {
public:
	Cone(glm::vec3 p, float r, float h, ofColor diffuse = ofColor::lightGray) {
		position = p; radius = r; height = h; diffuseColor = diffuse;
	}

	Cone() {};

	bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal) {
		return (glm::intersectRaySphere(ray.p, ray.d, position, radius, point, normal));
	}

	glm::vec3 getNormal(const glm::vec3& p) { return glm::normalize(p - position); }

	void draw() {
		ofPushMatrix();
		ofMultMatrix(getMatrix() * rotation);
		ofRotateDeg(180, 1, 0, 0);	// rotate so its base faces downward
		conePrim.setRadius(radius);
		conePrim.setHeight(height);
		conePrim.draw();
		ofPopMatrix();
	}

	string getType() override {
		return "cone";
	}

	float radius = 0.5;
	float height = 1.0;
	ofConePrimitive conePrim;
	glm::mat4 rotation = glm::mat4(1.0f);
};

class Light : public SceneObject {
public:

	Light(glm::vec3 p, float i = 0.5, ofColor d = ofColor::white) {
		position = p; intensity = i; diffuseColor = d;
	}

	Light() {};

	virtual void draw() {};

	virtual bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal) = 0;

	virtual int getSamples(glm::vec3 p, vector<Ray>& samples) = 0;

	string getType() override {
		return "light";
	}

	float intensity = 0.5;
};

class PointLight : public Light {
public:

	PointLight(glm::vec3 p, float i = 0.5, ofColor d = ofColor::white) {
		position = p;
		intensity = i;
		diffuseColor = d;
	};

	void draw() override { ofDrawSphere(position, 0.1); }

	bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal) override {
		return (glm::intersectRaySphere(ray.p, ray.d, position, 0.1, point, normal));
	}

	// getSamples() returns the light samples from the light
	// Pointlights generate just one sample.
		// p  is the rendering point on the surface
		// samples is the array of light sample which illuminate that point.
		// n is the number of light samples.
	int getSamples(glm::vec3 p, vector<Ray>& samples) override {
		// calculate ray from point to light source
		glm::vec3 direction = glm::normalize(position - p);
		samples.push_back(Ray(p, direction));
		return 1;  // only one sample for a point light
	}

	string getType() override {
		return "pointLight";
	}

	bool isSelectable = true;
};

class Spotlight : public Light {
public:

	Spotlight(Cone* cone, TargetSphere* sphere, float i = 0.85) {
		spotlight = cone;
		targetPoint = sphere;
		intensity = i;
		position = spotlight->position;
	};

	void draw() override {
		glm::vec3 direction = glm::normalize(targetPoint->position - spotlight->position);
		glm::mat4 rotationMatrix = rotateToVector(glm::vec3(0, -1, 0), direction);
		spotlight->rotation = rotationMatrix;

		spotlight->draw();		// spotlight
		targetPoint->draw();	// target point
		ofDrawLine(spotlight->position, targetPoint->position);	// connecting line
	}

	// placeholder, don't really need for spotlight
	bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal) override {
		return false;
	}

	// returns a transform matrix that rotates v1 onto v2
	glm::mat4 rotateToVector(glm::vec3 v1, glm::vec3 v2) {
		glm::vec3 axis = glm::cross(v1, v2);
		glm::quat q = glm::angleAxis(glm::angle(v1, v2), glm::normalize(axis));
		return glm::toMat4(q);
	}

	// getSamples() returns the light samples from the light
	// Spotlights generate one sample if the render point is illuminated by the spot beam
	// Otherwise, it generates 0 samples.

		// p  is the rendering point on the surface
		// samples is the array of light sample which illuminate that point.
		// n is the number of light samples.
	int getSamples(glm::vec3 p, vector<Ray>& samples) override {
		// calculate ray from point to light source
		glm::vec3 direction = glm::normalize(p - spotlight->position);

		// calculate light direction ray
		glm::vec3 lightDirection = glm::normalize(targetPoint->position - spotlight->position);

		// calculate cone light limits
			// calculate half cone angle (in radians)
		float coneAngle = atan(spotlight->radius / spotlight->height);

		// convert to range of -1 to 1 using cosine
		float coneAngleConverted = cos(coneAngle);

		float dotProd = glm::dot(direction, lightDirection);

		if (dotProd >= coneAngleConverted) {
			samples.push_back(Ray(p, direction));
			position = spotlight->position; // update position!
			return 1;  // only one sample for a point light
		}

		return 0;
	}

	string getType() override {
		return "spotlight";
	}

	Cone* spotlight;
	TargetSphere* targetPoint;

	bool isSelectable = true;
};


// view plane for render camera
//
class ViewPlane : public Plane {
public:
	ViewPlane(glm::vec2 p0, glm::vec2 p1) { min = p0; max = p1; }
	ViewPlane() { // create reasonable defaults (6x4 aspect)
		min = glm::vec2(-3, -2);
		max = glm::vec2(3, 2);
		position = glm::vec3(0, 0, 5);
		normal = glm::vec3(0, 0, 1); // viewplane currently limited to Z axis orientation
	}
	void setSize(glm::vec2 min, glm::vec2 max) {
		this->min = min;
		this->max = max;
	}
	float getAspect() { return width() / height(); }
	glm::vec3 toWorld(float u, float v); // (u, v) --> (x, y, z) [ world space ]
	void draw() {
		ofDrawRectangle(glm::vec3(min.x, min.y, position.z), width(), height());
	}
	float width() {
		return (max.x - min.x);
	}
	float height() {
		return (max.y - min.y);
	}

	// some convenience methods for returning the corners
	//
	glm::vec2 topLeft() { return glm::vec2(min.x, max.y); }
	glm::vec2 topRight() { return max; }
	glm::vec2 bottomLeft() { return min; }
	glm::vec2 bottomRight() { return glm::vec2(max.x, min.y); }
	
	glm::vec2 min, max;
};

class RenderCam : public SceneObject {
public:
	RenderCam() {
		position = glm::vec3(0, 0, 10);
		aim = glm::vec3(0, 0, -1);
	}
	Ray getRay(float u, float v);
	void draw() { ofDrawBox(position, 1.0); };
	void drawFrustum();
	glm::vec3 aim;
	ViewPlane view; // The camera viewplane, this is the view that we will render
};



class Triangle {
public:
	Triangle(int i, int j, int k) { this->i = i; this->j = j; this->k = k; }
	int i, j, k;
};

class Pyramid : public SceneObject {
public:
	Pyramid(ofColor color = ofColor::blue) {
		init();
		diffuseColor = color;
	}
	Pyramid(glm::vec3 tran, glm::vec3 rot, glm::vec3 sc, ofColor color = ofColor::blue) {
		init();
		position = tran;
		rotation = rot;
		scale = sc;
		diffuseColor = color;
	}
	void init();
	void draw();
	void setLength(float l) { len = l; init(); }
	bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal);

	float radius = 1.0;
	float len = 2.0;

	vector<glm::vec3> v;
	vector<Triangle> tri;
};

class Mesh : public SceneObject {
	public:

	Mesh() {}

	Mesh(string modelPath, glm::vec3 pos, ofColor diffuse = ofColor::red) {
		model.load(modelPath);
		position = pos;
		model.setPosition(position.x, position.y, position.z);
		diffuseColor = diffuse;
		isSelectable = true;

		syncMesh();
	}

	// recalculates normals (called during initialization + dragging)
	void syncMesh() {
		model.setPosition(position.x, position.y, position.z);
		modelMesh = model.getMesh(0);
		vertices = modelMesh.getVertices();
		indices = modelMesh.getIndices();

		calculateNormals();
	}

	void calculateNormals() {
		for (size_t i = 0; i < indices.size(); i += 3) {
			int ind1 = indices[i];							// get the indices of the current triangle's vertices
			int ind2 = indices[i + 1];
			int ind3 = indices[i + 2];

			glm::vec3 vert1 = vertices[ind1];				// use the indices to get the vertices of the curr triangle
			glm::vec3 vert2 = vertices[ind2];
			glm::vec3 vert3 = vertices[ind3];

			glm::vec3 edge1 = vert3 - vert1;				// calculate two edges from the vertices
			glm::vec3 edge2 = vert2 - vert1;

			glm::vec3 normal = glm::normalize(glm::cross(edge2, edge1)) * -1;	// calculate normal and add to vector
			triangleFaceNormals.push_back(normal);
    	}

	}

	bool intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal) {
		bool hit = false;
		float closestDist = numeric_limits<float>::max();

		for (size_t i = 0; i < indices.size(); i += 3) {
			int ind1 = indices[i];							// get the indices of the current triangle's vertices
			int ind2 = indices[i + 1];
			int ind3 = indices[i + 2];

			glm::vec3 vert1 = vertices[ind1];				// use the indices to get the vertices of the curr triangle
			glm::vec3 vert2 = vertices[ind2];
			glm::vec3 vert3 = vertices[ind3];

			// calculating the ray-triangle intersection
			float t;
			glm::vec2 baryPoint;
			bool intersects = glm::intersectRayTriangle(ray.p, ray.d, vert1, vert2, vert3, baryPoint, t);

			if (intersects && t > 0 && t < closestDist) {
				hit = true;
				closestDist = t;
				point = ray.p + ray.d * t;             		// calculate and save intersection point between ray and triangle
				normal = glm::normalize(glm::cross(vert2 - vert1, vert3 - vert1)); // Calculate triangle normal
			}
		}

		return hit;
	}

	glm::vec3 getNormal(const glm::vec3& p) {
		glm::vec3 rayOrigin = p;
		glm::vec3 rayDir = glm::vec3(0, 1, 0);
		glm::vec3 point, normal;
		float closestDist = numeric_limits<float>::max();
    	bool hit = false;

		for (size_t i = 0; i < indices.size(); i += 3) {
			int ind1 = indices[i];						// get the indices of the current triangle's vertices
			int ind2 = indices[i + 1];
			int ind3 = indices[i + 2];

			glm::vec3 v1 = vertices[ind1];				// use the indices to get the vertices of the curr triangle
			glm::vec3 v2 = vertices[ind2];
			glm::vec3 v3 = vertices[ind3];

			// check if the point p is inside the triangle using barycentric coordinates
			glm::vec2 bary;
			float t;
			
			if (glm::intersectRayTriangle(rayOrigin, rayDir, v1, v2, v3, bary, t)) {
				if (t < closestDist) {
					closestDist = t;
					normal = triangleFaceNormals[i/3];
					hit = true;
				}
			}
		}

		if (hit) {
			return glm::normalize(normal);
		}

    	return glm::vec3(0, 0, 0);						// if no triangle contains the point, return a zero vector
	}

	void draw() {
		ofPushMatrix();
    	ofTranslate(position); 							// Translate manually to the new position
    	modelMesh.draw();
    	ofPopMatrix();
	}

	string getType() override {
		return "mesh";
	}

	ofxAssimpModelLoader model;
	ofMesh modelMesh;
	vector<glm::vec3> vertices;
	vector<ofIndexType> indices;;
	vector<glm::vec3> triangleFaceNormals;
};




class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	bool mouseToDragPlane(int x, int y, glm::vec3& point);
	void mouseScrolled(int x, int y, float scrollX, float scrollY);

	static void drawAxis(glm::mat4 transform = glm::mat4(1.0), float len = 1.0);

	ofColor phonglambert(const glm::vec3& p, const glm::vec3& norm, const ofColor diffuse, const ofColor specular, float power);
	void rayTrace();
	vector<Light*> lights;
	vector<Spotlight*> spotlights;
	RenderCam renderCam;
	ofImage image;
	int imageWidth = 1200;	// 600
	int imageHeight = 800;	// 400
	bool bShowImage = false;


	// scene components
	//
	vector<SceneObject*> scene;
	vector<SceneObject*> selected;
	ofPlanePrimitive plane;

	// selection
	//
	bool objSelected() { return (selected.size() ? true : false); };
	void removeObject(SceneObject*);
	void clearSelectionList() {
		for (int i = 0; i < selected.size(); i++) {
			selected[i]->isSelected = false;
		}
		selected.clear();
	}


	// texture gui
	//
	void onTextureSelected(bool& value);


	// lights
	//
	ofLight light1;

	// cameras
	//
	ofEasyCam  mainCam;
	ofCamera sideCam;
	ofCamera topCam;
	ofCamera previewCam;
	ofCamera* theCam;    // set to current camera either mainCam or sideCam

	// materials
	//
	ofMaterial material;

	// state
	bool bDrag = false;
	bool bDisplayInstructions = true;
	bool bAltKeyDown = false;
	bool bSftKeyDown = false;
	bool bCtrlKeyDown = false;
	bool bRotateX = false;
	bool bRotateY = false;
	bool bRotateZ = false;
	bool bScaleX = false;
	bool bScaleY = false;
	bool bScaleZ = false;
	glm::vec3 lastPoint;

	// gui
	//
	ofxPanel lightGUI;						// for lights
	ofxFloatSlider lightIntensity;
	ofxFloatSlider powerPhong;
	bool lightInitialized = false;

	ofxPanel colorGUI;						// for rgb
	ofxColorSlider colorPicker;
	bool rgbInitialized = false;

	ofxPanel transformationGUI;				// for transformations
	ofxLabel divider1, divider2;
	ofxToggle aroundX, aroundY, aroundZ;	// rotate around axis
	ofxFloatSlider scaleX, scaleY, scaleZ;	// scale
	bool transfInitialized = false;

	ofxPanel textureGUI;
	map<string, ofParameter<bool>> textureOptions = {{"Blue Floral", false}, {"Yellow Floral", false}, 
													{"Marble", false},{"Stone", false}, {"Wood", false}};
	unordered_map<ofParameter<bool>*, string> toggleToTexture;
	bool textureInitialized = false;

};

