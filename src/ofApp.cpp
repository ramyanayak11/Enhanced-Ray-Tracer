// Ramya Nayak
// 
// Final Project - Enhanced Ray Tracer
// CS 116A - Introduction to Computer Graphics
//
// December 13, 2024



// Acknowledgements: 
//		Some starter code was provided by Professor Kevin Smith during past projects.



#include "ofApp.h"

// Intersect Ray with Plane  (wrapper on glm::intersect*)
//
bool Plane::intersect(const Ray& ray, glm::vec3& point, glm::vec3& normalAtIntersect) {
	float dist;
	bool insidePlane = false;
	bool hit = glm::intersectRayPlane(ray.p, ray.d, position, this->normal, dist);
	if (hit) {
		Ray r = ray;
		point = r.evalPoint(dist);
		normalAtIntersect = this->normal;
		glm::vec2 xrange = glm::vec2(position.x - width / 2, position.x + width / 2);
		glm::vec2 yrange = glm::vec2(position.y - width / 2, position.y + width / 2);
		glm::vec2 zrange = glm::vec2(position.z - height / 2, position.z + height / 2);

		// horizontal
		//
		if (normal == glm::vec3(0, 1, 0) || normal == glm::vec3(0, -1, 0)) {
			if (point.x < xrange[1] && point.x > xrange[0] && point.z < zrange[1] && point.z > zrange[0]) {
				insidePlane = true;
			}
		}
		// front or back
		//
		else if (normal == glm::vec3(0, 0, 1) || normal == glm::vec3(0, 0, -1)) {
			if (point.x < xrange[1] && point.x > xrange[0] && point.y < yrange[1] && point.y > yrange[0]) {
				insidePlane = true;
			}
		}
		// left or right
		//
		else if (normal == glm::vec3(1, 0, 0) || normal == glm::vec3(-1, 0, 0)) {
			if (point.y < yrange[1] && point.y > yrange[0] && point.z < zrange[1] && point.z > zrange[0]) {
				insidePlane = true;
			}
		}
	}

	ofImage background;
	float w = background.getWidth();
	float h = background.getHeight();
	return insidePlane;
}

void Pyramid::init() {

	v.clear();
	tri.clear();
	//
	// verts (local space)
	//
	v.push_back(glm::vec3(-radius, len / 2.0, radius));
	v.push_back(glm::vec3(radius, len / 2.0, radius));
	v.push_back(glm::vec3(radius, len / 2.0, -radius));
	v.push_back(glm::vec3(-radius, len / 2.0, -radius));
	v.push_back(glm::vec3(0, -len / 2.0, 0));

	// triangles (base)
	//
	tri.push_back(Triangle(0, 1, 3));
	tri.push_back(Triangle(1, 2, 3));

	// sides
	//
	tri.push_back(Triangle(0, 4, 1));
	tri.push_back(Triangle(1, 4, 2));
	tri.push_back(Triangle(2, 4, 3));
	tri.push_back(Triangle(3, 4, 0));

}

// Draw a Unit cube (size = 2) transformed
//
void Pyramid::draw() {

	glm::mat4 m = getMatrix();

	//   push the current stack matrix and multiply by this object's
	//   matrix. now all vertices will be transformed by this matrix
	//
	ofPushMatrix();
	ofMultMatrix(m);
	for (int i = 0; i < tri.size(); i++) {
		glm::vec3 v1 = v[tri[i].i];
		glm::vec3 v2 = v[tri[i].j];
		glm::vec3 v3 = v[tri[i].k];
		ofMaterial material;
		material.setDiffuseColor(diffuseColor);
		material.begin();
		ofDrawTriangle(v1, v2, v3);
		material.end();

		// draw normal (one each at vertex)
		//
		glm::vec3 normal = glm::cross(glm::normalize(v3 - v2), glm::normalize(v1 - v2));
		ofDrawLine(v1, v1 + (.2 * normal));
		ofDrawLine(v2, v2 + (.2 * normal));
		ofDrawLine(v3, v3 + (.2 * normal));
	}
	ofPopMatrix();

}


bool Pyramid::intersect(const Ray& ray, glm::vec3& point, glm::vec3& normal) {

	glm::mat4 m = getMatrix();
	for (int i = 0; i < tri.size(); i++) {

		// Compute normal (take cross product of two sides)
		//
		glm::vec3 v1 = m * glm::vec4(v[tri[i].i], 1.0);
		glm::vec3 v2 = m * glm::vec4(v[tri[i].j], 1.0);
		glm::vec3 v3 = m * glm::vec4(v[tri[i].k], 1.0);
		normal = glm::cross(glm::normalize(v3 - v2), glm::normalize(v1 - v2));

		// first check if ray is on correct side of triangle
		//
		if (glm::dot(ray.d, normal) >= 0.0)
			continue;

		// check ray/triangle intersection
		//
		glm::vec2 baryPoint;
		float t;
		if (glm::intersectRayTriangle(ray.p, ray.d, v1, v2, v3, baryPoint, t)) {
			point = ray.p + (ray.d * t);
			return true;
		}
	}
	return false;
}


//--------------------------------------------------------------
void ofApp::setup() {

	// setup background and cameras
	//
	ofSetBackgroundColor(ofColor::black);
	ofEnableDepthTest();
	mainCam.setDistance(15);
	mainCam.setNearClip(.1);
	mainCam.disableMouseInput();

	sideCam.setPosition(12, 0, 0);
	sideCam.lookAt(glm::vec3(0, 0, 0));
	topCam.setNearClip(.1);
	topCam.setPosition(0, 12, 0);
	topCam.lookAt(glm::vec3(0, 0, 0));
	ofSetFrameRate(24);

	ofSetSmoothLighting(true);

	// setup simple scene
	//
	scene.push_back(new Plane(glm::vec3(0, -2, 0), glm::vec3(0, 1, 0), ofColor(190.0, 133.0, 62.0), "", "", "", "wall_bump.jpeg", 3.0f));	// floor plane
	scene.push_back(new Plane(glm::vec3(0, 0, -6), glm::vec3(0, 0, 1), ofColor::darkGreen, "Yellow Floral", "floralWall2_diffuse.jpeg", "floralWall2_specular.jpeg", "", 15.0f));	// back wall plane
	scene.push_back(new Sphere(glm::vec3(-1, 0, 2), 1.0, ofColor::greenYellow));
	scene.push_back(new Sphere(glm::vec3(1.5, -0.5, 1), 1.0, ofColor::lightPink));
	scene.push_back(new Sphere(glm::vec3(0, 0.5, 0), 1.0, ofColor::cornflowerBlue, "Marble", "marbleFloor_diffuse.jpeg", "marbleFloor_specular.jpeg", 1.0f));
	scene.push_back(new Mesh("cherry.obj", glm::vec3(0.0, 0.0, 0.0), ofColor::red));


	// setup one point light
	//
	light1.enable();
	light1.setPosition(0, 100, 0);
	light1.setDiffuseColor(ofColor(255.f, 255.f, 255.f));
	light1.setSpecularColor(ofColor(255.f, 255.f, 255.f));
	light1.setAmbientColor(ofColor(150, 150, 150));

	theCam = &mainCam;

	image.allocate(imageWidth, imageHeight, OF_IMAGE_COLOR);


	// light sliders
	//
	lightGUI.setup("Light Controls");
	lightGUI.add(lightIntensity.setup("Light intensity", 0.6, 0.0, 5.0));
	lightGUI.add(powerPhong.setup("Power exponent", 35.0, 10.0, 500.0));

	// rgb sliders
	//
	colorGUI.setup("Change Color");
	colorGUI.add(colorPicker.setup("", ofColor(255, 0, 0), ofColor(0, 0, 0), ofColor(255, 255, 255)));

	// transformation sliders
	//
	transformationGUI.setup("Modify Object");
	transformationGUI.add(divider1.setup("", "Rotate around axis"));
	transformationGUI.add(aroundX.setup("X-axis", false));
	transformationGUI.add(aroundY.setup("Y-axis", false));
	transformationGUI.add(aroundZ.setup("Z-axis", false));
	transformationGUI.add(divider2.setup("", "Scale"));
	transformationGUI.add(scaleX.setup("X-axis", 1.0, 0.0, 10.0));					// default=1 , min=0 , max=10
	transformationGUI.add(scaleY.setup("Y-axis", 1.0, 0.0, 10.0));
	transformationGUI.add(scaleZ.setup("Z-axis", 1.0, 0.0, 10.0));

	// texture gui
	//
	textureGUI.setup("Change Texture");
	for (auto& option : textureOptions) {
		textureGUI.add(option.second.set(option.first, false)); 	// each texture default to false

        option.second.addListener(this, &ofApp::onTextureSelected);	// add listener to each toggle

		// adding an entry to toggleToTexture where key = bool and value = texture name 
        toggleToTexture[&option.second] = option.first; 
	}
	
}

//--------------------------------------------------------------
void ofApp::update() {

	// set gui panel positions
	colorGUI.setPosition(10, ofGetHeight() - colorGUI.getHeight() - 10);											// left-bottom
	transformationGUI.setPosition(10, (ofGetHeight() / 2) - (transformationGUI.getHeight() / 2) - 30);				// left-center
	lightGUI.setPosition(ofGetWidth() - lightGUI.getWidth() - 10, 10);												// right-top
	textureGUI.setPosition(ofGetWidth() - textureGUI.getWidth() - 10, ofGetHeight() - textureGUI.getHeight() - 10);	// right-bottom

	// ---------------------

	if (objSelected()) {
		float rotAngle = 2.0;		// rotation speed

		if (aroundX == true) {		// rotate around x axis
			glm::vec3 pivotPoint = glm::vec3(selected[0]->position.x, 0, 0);
			glm::vec3 rotAxis = glm::vec3(1, 0, 0);

			glm::vec3 relativePosition = selected[0]->position - pivotPoint;
			glm::vec3 rotatedPosition = glm::rotate(relativePosition, glm::radians(rotAngle), rotAxis);

			selected[0]->position = rotatedPosition + pivotPoint;
		}
		if (aroundY == true) {		// rotate around y axis
			glm::vec3 pivotPoint = glm::vec3(0, selected[0]->position.y, 0);
			glm::vec3 rotAxis = glm::vec3(0, 1, 0);

			glm::vec3 relativePosition = selected[0]->position - pivotPoint;
			glm::vec3 rotatedPosition = glm::rotate(relativePosition, glm::radians(rotAngle), rotAxis);

			selected[0]->position = rotatedPosition + pivotPoint;

		}
		if (aroundZ == true) {		// rotate around z axis
			glm::vec3 pivotPoint = glm::vec3(0, 0, selected[0]->position.z);
			glm::vec3 rotAxis = glm::vec3(0, 0, 1);

			glm::vec3 relativePosition = selected[0]->position - pivotPoint;
			glm::vec3 rotatedPosition = glm::rotate(relativePosition, glm::radians(rotAngle), rotAxis);

			selected[0]->position = rotatedPosition + pivotPoint;
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	theCam->begin();
	ofEnableLighting();
	ofEnableDepthTest();
	ofFill();

	// draw the scene. if an object is selected, draw in white - otwherwise draw in it's diffuse color.
	for (int i = 0; i < scene.size(); i++) {
		if (scene[i]->isSelected)
			ofSetColor(ofColor::white);
		else ofSetColor(scene[i]->diffuseColor);
		scene[i]->draw();
	}

	// draw the default light
	light1.setScale(.1, .1, .1);
	ofSetColor(ofColor::grey);
	light1.draw();

	ofDisableLighting();

	// adding frustum
	ofSetColor(ofColor::lightSkyBlue);
	ofNoFill();
	renderCam.drawFrustum();
	renderCam.view.draw();
	ofSetColor(ofColor::blue);
	renderCam.draw();
	ofFill();

	theCam->end();

	if (bShowImage) {
		image.load("rendered_image.png");
		ofSetColor(255);
		image.draw((ofGetWidth() - imageWidth) / 2, (ofGetHeight() - imageHeight) / 2, imageWidth, imageHeight);
	}

	// gui (only shown when an object is selected)
	if (objSelected()) {

		ofDisableDepthTest();

		if (!rgbInitialized) {
			// set the starting values of RGB sliders to the object's current color
			// use a boolean flag because we only want to do this once
			colorPicker = (selected[0]->diffuseColor);

			rgbInitialized = true;
		}

		if (!transfInitialized) {
			scaleX = (selected[0]->scale.x);
			scaleY = (selected[0]->scale.y);
			scaleZ = (selected[0]->scale.z);

			transfInitialized = true;
		}

		// rgb / transformations gui
		colorGUI.draw();
		transformationGUI.draw();

		// update color
		selected[0]->diffuseColor = colorPicker;

		// update scale
		selected[0]->scale = glm::vec3(float(scaleX), float(scaleY), float(scaleZ));


		// light gui (only shown when a light object is selected)
		if (selected[0]->getType() == "light" || selected[0]->getType() == "pointLight" || selected[0]->getType() == "spotlight") {
			Light* light = static_cast<Light*>(selected[0]);	// to access vars specific to the light class

			if (!lightInitialized) {
				lightIntensity = light->intensity;
				lightInitialized = true;
			}

			lightGUI.draw();

			light->intensity = lightIntensity;
		}
		else {
			lightInitialized = false;
		}
	}
	else {
		// reset boolean flags
		rgbInitialized = false;
		transfInitialized = false;
	}

	// show gui for changing textures only if selected obj is textured
	if (objSelected() && selected[0]->textureName != "") {
		ofDisableDepthTest();

		if(!textureInitialized) {
			for (auto& option : textureOptions) {
				option.second = (option.first == selected[0]->textureName);
			}
			textureInitialized = true;
		}

		textureGUI.draw();
	}
	else {
		textureInitialized = false;
	}



	// displaying text with ui instructions
	if (bDisplayInstructions) {
		ofSetColor(ofColor::white);
		string str;
		str += "Instructions \n\n";
		str += "Hold alt and drag to move camera position\n";
		str += "'q' saves current camera position as the render cam\n";
		str += "'s' adds a sphere\n";
		str += "'p' adds a pyramid\n";
		str += "'l' adds a point light\n";
		str += "'t' adds a spotlight\n";
		str += "'r' renders scene\n";
		str += "'i' displays rendered image\n";
		str += "'h' toggles on/off these instructions\n";
		ofDrawBitmapString(str, 20, 20);
	}

}


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
	case 'C':
	case 'c':
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'h':
		bDisplayInstructions = !bDisplayInstructions;
		break;
	case 'i':
		bShowImage = !bShowImage;
		break;

	case 'l':
	{
		PointLight* pointLight = new PointLight(glm::vec3(0, 10, 5));
		scene.push_back(pointLight);
		lights.push_back(pointLight);
		break;
	}
	case 'p':
	{
		Pyramid* pyramid = new Pyramid();
		scene.push_back(pyramid);
		break;
	}
	case 'q': {
		// update render cam
		renderCam.position = mainCam.getPosition();
		renderCam.aim = glm::normalize(mainCam.getLookAtDir());

		// update view plane
		renderCam.view.position = renderCam.position + renderCam.aim * 5.0f;
		renderCam.view.normal = renderCam.aim;

		glm::vec3 camRight = glm::normalize(mainCam.getSideDir()); // Right vector of the camera
		glm::vec3 camUp = glm::normalize(mainCam.getUpDir());      // Up vector of the camera

		// Define the corners of the ViewPlane
		float width = 6.0f;  // Total width of the view plane (e.g., from -3 to +3)
		float height = 4.0f; // Total height of the view plane (e.g., from -2 to +2)

		renderCam.view.setSize(
			renderCam.view.position - camRight * (width / 2.0f) - camUp * (height / 2.0f), // Bottom-left corner
			renderCam.view.position + camRight * (width / 2.0f) + camUp * (height / 2.0f)  // Top-right corner
		);

		renderCam.draw();
		renderCam.view.draw();
		break;
	}
	case 'r':
		rayTrace();
		cout << "done..." << endl;
		break;
	case 's':
	{
		Sphere* sphere = new Sphere();
		scene.push_back(sphere);
		break;
	}
	case 't':
	{
		Cone* cone = new Cone(glm::vec3(0, 5, 0), 0.5, 1.0);
		TargetSphere* sphere = new TargetSphere(glm::vec3(0.01, 3, 0), 0.25);	// slightly off center

		Spotlight* spotlight = new Spotlight(cone, sphere);
		scene.push_back(spotlight);
		scene.push_back(spotlight->spotlight);
		scene.push_back(spotlight->targetPoint);

		lights.push_back(spotlight);
		spotlights.push_back(spotlight);
		break;
	}
	break;

	case 'x':
		bRotateX = true;
		break;
	case 'y':
		bRotateY = true;
		break;
	case 'z':
		bRotateZ = true;
		break;

	case OF_KEY_F1:
		theCam = &mainCam;
		break;
	case OF_KEY_F2:
		theCam = &sideCam;
		break;
	case OF_KEY_F3:
		theCam = &topCam;
		break;
	case OF_KEY_F4:
		theCam = &previewCam;
		break;
	case OF_KEY_ALT:
		bAltKeyDown = true;
		if (!mainCam.getMouseInputEnabled()) mainCam.enableMouseInput();
		break;
	case OF_KEY_SHIFT:
		bSftKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_BACKSPACE:
		if (objSelected()) {
			removeObject(selected[0]);
			clearSelectionList();
		}
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	switch (key) {
	case OF_KEY_ALT:
		bAltKeyDown = false;
		mainCam.disableMouseInput();
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		break;
	case 'x':
		bRotateX = false;
		break;
	case 'y':
		bRotateY = false;
		break;
	case 'z':
		bRotateZ = false;
		break;

	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

void ofApp::removeObject(SceneObject* obj) {

	// remove from selection list
	//
	for (int i = 0; i < scene.size(); i++) {
		if (scene[i] == obj) {
			scene.erase(scene.begin() + i);
			break;
		}
	}

	// remove from scene list;
	//
	for (int i = 0; i < scene.size(); i++) {
		if (scene[i] == obj) {
			scene.erase(scene.begin() + i);
			break;
		}
	}

	// delete object;  sceneObject destructor will handle cleaning up parent and child lists.
	//
	delete obj;
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	if (objSelected() && bDrag) {
		glm::vec3 point;
		mouseToDragPlane(x, y, point);

		if (bRotateX) {
			selected[0]->rotation += glm::vec3((point.x - lastPoint.x) * 20.0, 0, 0);
		}
		else if (bRotateY) {
			selected[0]->rotation += glm::vec3(0, (point.x - lastPoint.x) * 20.0, 0);
		}
		else if (bRotateZ) {
			selected[0]->rotation += glm::vec3(0, 0, (point.x - lastPoint.x) * 20.0);
		}
		else {
			selected[0]->position += (point - lastPoint);
			if (selected[0]->getType() == "mesh") {
				Mesh* mesh = dynamic_cast<Mesh*>(selected[0]);
				mesh->syncMesh();				// updates mesh after dragging
			}
		}
		lastPoint = point;
	}

}

bool ofApp::mouseToDragPlane(int x, int y, glm::vec3& point) {
	glm::vec3 p = theCam->screenToWorld(glm::vec3(x, y, 0));
	glm::vec3 d = p - theCam->getPosition();
	glm::vec3 dn = glm::normalize(d);

	float dist;
	glm::vec3 pos;
	if (objSelected()) {
		pos = selected[0]->position;
	}
	else pos = glm::vec3(0, 0, 0);
	if (glm::intersectRayPlane(p, dn, pos, glm::normalize(theCam->getZAxis()), dist)) {
		point = p + dn * dist;
		return true;
	}
	return false;
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	rgbInitialized = false;
	lightInitialized = false;
	transfInitialized = false;
	textureInitialized = false;

	// if we are moving the camera around, don't allow selection
	if (mainCam.getMouseInputEnabled()) return;

	// clear selection list
	clearSelectionList();

	// test if something selected
	vector<SceneObject *> hits;

	glm::vec3 p = theCam->screenToWorld(glm::vec3(x, y, 0));
	glm::vec3 d = p - theCam->getPosition();
	glm::vec3 dn = glm::normalize(d);

	// check for selection of scene objects
	for (int i = 0; i < scene.size(); i++) {
		
		glm::vec3 point, norm;
		
		//  We hit an object
		if (scene[i]->isSelectable && scene[i]->intersect(Ray(p, dn), point, norm)) {
			hits.push_back(scene[i]);
		}
	}

	// if we selected more than one, pick nearest
	SceneObject *selectedObj = NULL;
	float nearestDist = std::numeric_limits<float>::infinity();

	if (hits.size() > 0) {
		selectedObj = hits[0];
		for (int n = 0; n < hits.size(); n++) {
			float dist = glm::length(hits[n]->position - theCam->getPosition());
			if (dist < nearestDist) {
				nearestDist = dist;
				selectedObj = hits[n];
			}	
		}
	}
	if (selectedObj) {
		selectedObj->isSelected = true;
		selected.push_back(selectedObj);
		bDrag = true;
		mouseToDragPlane(x, y, lastPoint);
	}
	else {
		clearSelectionList();
	}

}

//--------------------------------------------------------------

void ofApp::mouseReleased(int x, int y, int button) {
	bDrag = false;
}

// Draw an XYZ axis in RGB at transform
//
void ofApp::drawAxis(glm::mat4 m, float len) {

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(glm::vec3(m * glm::vec4(0, 0, 0, 1)), glm::vec3(m * glm::vec4(len, 0, 0, 1)));


	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(glm::vec3(m * glm::vec4(0, 0, 0, 1)), glm::vec3(m * glm::vec4(0, len, 0, 1)));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(glm::vec3(m * glm::vec4(0, 0, 0, 1)), glm::vec3(m * glm::vec4(0, 0, len, 1)));
}


void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {
	for (Spotlight* spotlight : spotlights) {
		if (scrollY > 0) {
			// scrolling up
			spotlight->spotlight->radius += 0.1;
		}
		if (scrollY < 0) {
			// scrolling down
			spotlight->spotlight->radius -= 0.1;
		}
	}

}

//		p is the point of intersection
//		norm is the normal
//		diffuse is the diffuse color of the scene object
//		specular is the specular color (color of your highlights)
//		power is the phong power exponent(10 - 10000) --> NOTE: use glm::pow(x,y)
ofColor ofApp::phonglambert(const glm::vec3& p, const glm::vec3& norm, const ofColor diffuse, const ofColor specular, float power) {

	ofColor pixelColor = ofColor::black; // initialize pixel to black

	// ambient light
	pixelColor += diffuse * ofColor::white * 0.3;

	for (Light* light : lights) {

		vector<Ray> samples;
		if (light->getSamples(p, samples) == 0) {
			continue;
		}

		glm::vec3 v = glm::normalize(renderCam.position - p);	// get unit vector pointing toward the camera

		glm::vec3 l = glm::normalize(light->position - p);		// get unit vector pointing towards light source

		glm::vec3 h = glm::normalize(v + l);					// calculate half vector, aka the bisector of the angle between v and l

		Ray shadowRay(p + l * 0.7, l);		// calculate shadow ray (from intersection to light source)
		// (l is used so that p is shifted in the correct direction)

		float distanceFromIntersectionToLight = glm::distance(light->position, p);

		bool inShadow = false;

		for (SceneObject* obj : scene) {
			glm::vec3 intersectionPoint, intersectionNormal;

			if (obj->getType() == "light" || obj->getType() == "pointLight" || obj->getType() == "spotlight" || obj->getType() == "cone" || obj->getType() == "targetSphere") {	// ignore intersections with lights
				continue;
			}

			// add shadows (if shadow ray is blocked, that pixel is in shadow)
			if (obj->intersect(shadowRay, intersectionPoint, intersectionNormal)) {
				float distanceFromShadowIntersectionToLight = glm::distance(light->position, intersectionPoint);
				if (distanceFromShadowIntersectionToLight < distanceFromIntersectionToLight) {
					inShadow = true;
					break;
				}
			}
		}

		if (!inShadow) {			// lightIntensity							// light->diffuseColor  // lightIntensity
			pixelColor += (diffuse * light->intensity * max(0.0f, glm::dot(norm, l))) + specular * light->intensity * glm::pow(max(0.0f, glm::dot(norm, h)), power);
		}
	}

	return pixelColor;
}

void ofApp::rayTrace() {

	// for every pixel on the screen
	for (int y = 0; y < imageHeight; y++) {
		for (int x = 0; x < imageWidth; x++) {
			// normalize x and y coordinates to be on 0-1 scale
			float u = (x + 0.5) / imageWidth;
			float v = 1.0 - ((y + 0.5) / imageHeight);

			// calculate view ray
			Ray ray = renderCam.getRay(u, v);

			glm::vec3 intersection, normal;
			SceneObject* closestObj = nullptr;
			ofColor color = ofColor::black;	// default black color

			float closestDist = numeric_limits<float>::max();	// initialize to a large number

			for (int i = 0; i < scene.size(); ++i) {

				glm::vec3 intersectionPoint, intersectionNormal;
				float distance;

				// find the closest obj that intersects the view ray
				if (scene[i]->getType() != "targetSphere" && scene[i]->intersect(ray, intersectionPoint, intersectionNormal)) {

					distance = glm::distance(ray.p, intersectionPoint);

					if (distance < closestDist) {
						closestDist = distance;
						closestObj = scene[i];
						intersection = intersectionPoint;
						normal = intersectionNormal;
					}
				}

			}

			// if viewing ray is intersected with an object
			if (closestObj != nullptr) {
				// calculate color of the pixel
				if (closestObj->getType() == "light" || closestObj->getType() == "pointLight" || closestObj->getType() == "spotlight" || closestObj->getType() == "cone") {
					// if object is a light, make it the color of its specular/diffuse (I used the same colors for both)
					color = closestObj->diffuseColor;
				}
				else {
					ofColor textureDiffuse = closestObj->diffuseColor;
					ofColor textureSpecular = ofColor::white;
					glm::vec3 bumpNormal = normal;
					if (closestObj->getType() == "plane") {
						Plane* plane = dynamic_cast<Plane*>(closestObj);
						textureDiffuse = plane->textureLookup(&plane->texture, intersection);
						textureSpecular = plane->textureLookup(&plane->specular, intersection);
						bumpNormal = plane->bumpLookup(&plane->bump, intersection, normal);
					}
					if (closestObj->getType() == "sphere") {
						Sphere* sphere = dynamic_cast<Sphere*>(closestObj);
						textureDiffuse = sphere->textureLookup(&sphere->texture, intersection);
						textureSpecular = sphere->textureLookup(&sphere->specular, intersection);
					}

					// closestObj->diffuseColor
					color = phonglambert(intersection, bumpNormal, textureDiffuse, textureSpecular, powerPhong);
				}
			}

			// set the pixel to the chosen color (black if there was no intersected object)
			image.setColor(x, y, color);
		}
	}

	image.save("rendered_image.png");	// save image

}

ofColor Plane::textureLookup(ofImage* t, glm::vec3 intersection) {
	// if no texture is loaded, just return diffuse color
	if (!t->isAllocated()) { return diffuseColor; }


	// map the intersection point to the plane’s local coordinate system
	//	(according to its normal)

	float u = 0.0f;
	float v = 0.0f;

	if (normal == glm::vec3(0, 1, 0)) {		// horizontal ground plane
		u = (intersection.x - position.x + width / 2.0f) / width;
		v = (intersection.z - position.z + height / 2.0f) / height;
	}
	else if (normal == glm::vec3(0, -1, 0)) {
		u = (intersection.x - position.x + width / 2.0f) / width;
		v = 1.0f - (intersection.z - position.z + height / 2.0f) / height;
	}
	else if (normal == glm::vec3(0, 0, 1)) {	// vertical wall plane
		u = (intersection.x - position.x + width / 2.0f) / width;
		v = (intersection.y - position.y + height / 2.0f) / height;
	}
	else if (normal == glm::vec3(0, 0, -1)) {
		u = 1.0f - (intersection.x - position.x + width / 2.0f) / width;
		v = (intersection.y - position.y + height / 2.0f) / height;
	}
	else if (normal == glm::vec3(1, 0, 0)) {	// left wall plane
		u = (intersection.z - position.z + height / 2.0f) / height;
		v = (intersection.y - position.y + width / 2.0f) / width;
	}
	else if (normal == glm::vec3(-1, 0, 0)) {	// right wall plane
		u = 1.0f - (intersection.z - position.z + height / 2.0f) / height;
		v = (intersection.y - position.y + width / 2.0f) / width;
	}

	// fmod to control tiling

	u *= tilingFactor;
	v *= tilingFactor;

	u = fmod(u, 1.0f);
	v = fmod(v, 1.0f);

	// convert (u, v) texture coordinates into (i,j) pixel coordinates
	int i = static_cast<int>(round(u * t->getWidth() - 0.5));
	int j = static_cast<int>(round(v * t->getHeight() - 0.5));

	// clamp i and j
	i = max(0, min(i, int(t->getWidth()) - 1));
	j = max(0, min(j, int(t->getHeight()) - 1));

	// added wrapping for repeating/seamless textures
	return t->getColor(i % int(t->getWidth()), j % int(t->getHeight()));
}

ofColor Sphere::textureLookup(ofImage* t, glm::vec3 intersection) {
	// if no texture is loaded, just return diffuse color
	if (!t->isAllocated()) { return diffuseColor; }

	// calculate the normal at the intersection point
	glm::vec3 normal = getNormal(intersection);

	// compute spherical coordinates
	float theta = atan2(normal.z, normal.x);	// longitude
	float phi = acos(normal.y);					// latitude

	// map to texture coordinates
	float u = 0.5f + (theta / (2 * PI)); // map theta from [-PI, PI] to [0, 1]
	float v = 1 - (phi / PI);            // map phi from [0, PI] to [0, 1]

	// fmod to control tiling

	u *= tilingFactor;
	v *= tilingFactor;

	u = fmod(u, 1.0f);
	v = fmod(v, 1.0f);

	// convert (u, v) texture coordinates into (i,j) pixel coordinates
	int i = round(u * t->getWidth() - 0.5);
	int j = round(v * t->getHeight() - 0.5);
	

	// clamp i and j
	i = max(0, min(i, int(t->getWidth()) - 1));
	j = max(0, min(j, int(t->getHeight()) - 1));

	// added wrapping for repeating/seamless textures
	return t->getColor(i % int(t->getWidth()), j % int(t->getHeight()));
}

glm::vec3 Plane::bumpLookup(ofImage* b, glm::vec3 intersection, glm::vec3 originalNormal) {
	if (!b->isAllocated()) { return originalNormal; }

	// map the intersection point to the plane’s local coordinate system
	//	(according to its normal)

	float u = 0.0f;
	float v = 0.0f;
	glm::vec3 tan, bitan;	// for calculating tangent and bitangent later

	if (originalNormal == glm::vec3(0, 1, 0)) {			// horizontal ground plane
		u = (intersection.x - position.x + width / 2.0f) / width;
		v = (intersection.z - position.z + height / 2.0f) / height;
		tan = glm::vec3(1, 0, 0);
		bitan = glm::vec3(0, 0, 1);
	}
	else if (originalNormal == glm::vec3(0, -1, 0)) {
		u = (intersection.x - position.x + width / 2.0f) / width;
		v = 1.0f - (intersection.z - position.z + height / 2.0f) / height;
		tan = glm::vec3(1, 0, 0);
		bitan = glm::vec3(0, 0, -1);
	}
	else if (originalNormal == glm::vec3(0, 0, 1)) {	// vertical wall plane
		u = (intersection.x - position.x + width / 2.0f) / width;
		v = (intersection.y - position.y + height / 2.0f) / height;
		tan = glm::vec3(1, 0, 0);
		bitan = glm::vec3(0, 1, 0);
	}
	else if (originalNormal == glm::vec3(0, 0, -1)) {
		u = 1.0f - (intersection.x - position.x + width / 2.0f) / width;
		v = (intersection.y - position.y + height / 2.0f) / height;
		tan = glm::vec3(1, 0, 0);
		bitan = glm::vec3(0, -1, 0);
	}
	else if (originalNormal == glm::vec3(1, 0, 0)) {	// left wall plane
		u = (intersection.z - position.z + height / 2.0f) / height;
		v = (intersection.y - position.y + width / 2.0f) / width;
		tan = glm::vec3(0, 0, 1);
		bitan = glm::vec3(0, 1, 0);
	}
	else if (originalNormal == glm::vec3(-1, 0, 0)) {	// right wall plane
		u = 1.0f - (intersection.z - position.z + height / 2.0f) / height;
		v = (intersection.y - position.y + width / 2.0f) / width;
		tan = glm::vec3(0, 0, 1);
		bitan = glm::vec3(0, -1, 0);
	}

	// fmod to control tiling
	u *= tilingFactor;
	v *= tilingFactor;

	u = fmod(u, 1.0f);
	v = fmod(v, 1.0f);

	// convert (u, v) texture coordinates into (i,j) pixel coordinates
	int i = round(u * b->getWidth() - 0.5);
	int j = round(v * b->getHeight() - 0.5);

	// clamp i and j
	i = max(0, min(i, int(b->getWidth()) - 1));
	j = max(0, min(j, int(b->getHeight()) - 1));

	// brightness at pixel and neighboring pixels (mod for tiling + out of bounds error)
	float brightness = b->getColor(i, j).getBrightness(); 								
    float bRight = b->getColor((i + 1) % (int)(b->getWidth()), j).getBrightness();
    float bUp = b->getColor(i, (j + 1) % (int)(b->getHeight())).getBrightness();

	// controls bump intensity (greater value = more pronounced bumps)
    float scale = 0.01f; 					

	// height diff between curr pixel and pixels at right and above
    float du = (bRight - brightness) * scale;
    float dv = (bUp - brightness) * scale;

	// apply bump effect using the calculations above
	glm::vec3 tangent = glm::normalize(tan);
    glm::vec3 bitangent = glm::normalize(bitan);
    glm::vec3 perturbedNormal = normal + (du * tangent) + (dv * bitangent);	

	return glm::normalize(perturbedNormal);
}

void ofApp::onTextureSelected(bool& value) {
	
    if (value) {
        // find the texture name corresponding to the selected toggle
        for (auto& option : textureOptions) {
            if (&option.second.get() == &value) {
                string selectedTextureName = option.first;

                // iterate through and deselect all other toggles
                for (auto& otherOption : textureOptions) {
                    if (otherOption.first != selectedTextureName) {
                        otherOption.second = false;
                    }
                }

                // change the texture of the selected object according to the selected toggle
                if (objSelected()) {
                    if (selectedTextureName == "Blue Floral") {
						selected[0]->changeTexture(selectedTextureName, "floralWall1_diffuse.jpeg", "floralWall1_specular.jpeg");
					}
					else if (selectedTextureName == "Yellow Floral") {
						selected[0]->changeTexture(selectedTextureName, "floralWall2_diffuse.jpeg", "floralWall2_specular.jpeg");
					}
					else if (selectedTextureName == "Marble") {
						selected[0]->changeTexture(selectedTextureName, "marbleFloor_diffuse.jpeg", "marbleFloor_specular.jpeg");
					}
					else if (selectedTextureName == "Stone") {
						selected[0]->changeTexture(selectedTextureName, "stoneWall_diffuse.jpeg", "stoneWall_specular.jpeg");
					}
					else if (selectedTextureName == "Wood") {
						selected[0]->changeTexture(selectedTextureName, "woodFloor_diffuse.jpeg", "woodFloor_specular.jpeg");
					}
                }

            }
        }
    }
}



// ----------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------



// Convert (u, v) to (x, y, z)
// We assume u,v is in [0, 1]
//
glm::vec3 ViewPlane::toWorld(float u, float v) {
	float w = width();
	float h = height();
	return (glm::vec3((u * w) + min.x, (v * h) + min.y, position.z));
}

// Get a ray from the current camera position to the (u, v) position on
// the ViewPlane
//
Ray RenderCam::getRay(float u, float v) {
	glm::vec3 pointOnPlane = view.toWorld(u, v);
	return(Ray(position, glm::normalize(pointOnPlane - position)));
}

void RenderCam::drawFrustum() {
	Ray r1 = getRay(0, 0);
	Ray r2 = getRay(0, 1);
	Ray r3 = getRay(1, 1);
	Ray r4 = getRay(1, 0);
	float dist = glm::length((view.toWorld(0, 0) - position));
	r1.draw(dist);
	r2.draw(dist);
	r3.draw(dist);
	r4.draw(dist);
}
