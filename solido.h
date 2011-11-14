#define LINES 3000.0 /* MAX lines to draw*/
#define SIZE 2 
/* Estructura de Punto*/ 
struct point3D{ 
	 float x; 
	 float y; 
	 float z; 
}; 

/*Silueta de una Lampara */ 
struct point3D v[SIZE] = {
	{0.0,0.0,0.0},
	{2.0,-1.0,0.0}	
};

/* Dibuja silueta*/ 
void drawOutline() { 
	int i; 
	glBegin(GL_LINE_STRIP);
	for( i=0; i<SIZE; ++i){ 
		glVertex3f( v[i].x, v[i].y, v[i].z ); 
	} 
	glEnd();

} 

/* Revoluciona la silueta */ 
void drawRev(){
	/*
	int i; 
	for( i=0;i<LINES;++i) { 
		float frac = (float)i / LINES;
		glRotatef(frac*360,0.0,1.0,0.0);
		glColor3f(1.0,0.0,0.0);
		drawOutline(); 
	}
	*/
float vertices[20][36][3];
int Latitudes=20;
int Longitudes=36;

for(int latCount=0; latCount<Latitudes; latCount++)
    { // along z axis
    for(int longCount=0; longCount<Longitudes; longCount++){ // x and y
      float z = sin(M_PI*latCount/((float) (Latitudes-1))-M_PI/2.0f)/5;
      float r = cos(M_PI*latCount/((float) (Latitudes-1))-M_PI/2.0f);
      float x = (cos(M_PI*longCount/(float) (Longitudes-1))*r) /5;
      float y = (sin(M_PI*longCount/(float) (Longitudes-1))*r *2) /5;

      vertices[latCount][longCount][0] = x; //x
      vertices[latCount][longCount][1] = y; //y
      vertices[latCount][longCount][2] = z; //z
}}

for(int latCount=0; latCount<Latitudes-1; latCount++){ // along z axis
    glBegin(GL_QUAD_STRIP);
        for(int longCount=0; longCount<=Longitudes; longCount++){
            glVertex3f(vertices[latCount][longCount][0], vertices[latCount][longCount][1], vertices[latCount][longCount][2]);
            glVertex3f(vertices[latCount+1][longCount][0], vertices[latCount+1][longCount][1], vertices[latCount+1][longCount][2]);
        }

    glEnd();
}
} 


