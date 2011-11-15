#define LINES 3000.0 /* MAX lines to draw*/
#define SIZE 3 
/* Estructura de Punto*/ 
struct point3D{ 
	 float x; 
	 float y; 
	 float z; 
}; 

/*Silueta de una Lampara */ 
struct point3D v[SIZE] = {
	{0.0,0.0,0.0},
	{1.5,-1.0,0.0},
	{1.8, -0.7, 0.0}
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
	int i; 
	for( i=0;i<LINES;++i) { 
		float frac = (float)i / LINES; /* Recorrera los 360 Grados Divididos en el numero de Lineas que se desea dibujar.*/ 
		glRotatef(frac*360,0.0,1.0,0.0); 
		drawOutline(); 
	} 
} 


float *calculaNormal(float p1[3],float p2[3],float p3[3]){
	float v1[3]={0};
	float v2[3]={0};
	float *v3=(float *)malloc(sizeof(float)*3);

	//Calculo del Producto Cruz y los vectores.
	v1[0]=p2[0]-p1[0];
	v1[1]=p2[1]-p1[1];
	v1[2]=p2[2]-p1[2];

	v2[0]=p3[0]-p1[0];
	v2[1]=p3[1]-p1[1];
	v2[2]=p3[2]-p1[2];

	v3[0]=v1[1]*v2[2]-v1[2]*v2[1];
	v3[1]=v1[2]*v2[0]-v1[0]-v2[2];
	v3[2]=v1[0]*v2[1]-v1[1]*v2[0];

	//Calculo de Distancia
	float length = sqrt((v3[0]*v3[0])+(v3[1]*v3[1])+(v3[2]*v3[2]));

	//Vector Unitario
	v3[0]=v3[0]/length;
	v3[1]=v3[1]/length;
	v3[2]=v3[2]/length;
	
	return v3;
}

/* Media Esfera*/ 
void drawHS(){
	float vertices[20][36][3];
	int Latitudes=20;
	int Longitudes=36;

	for(int latCount=0; latCount<Latitudes; latCount++){ 
	    for(int longCount=0; longCount<Longitudes; longCount++){ // x and y
	      float z = sin(M_PI*latCount/((float) (Latitudes-1))-M_PI/2.0f)/5;
	      float r = cos(M_PI*latCount/((float) (Latitudes-1))-M_PI/2.0f);
	      float x = (cos(M_PI*longCount/(float) (Longitudes-1))*r) /5;
	      float y = (sin(M_PI*longCount/(float) (Longitudes-1))*r *2) /5;
	
	      vertices[latCount][longCount][0] = x; //x
	      vertices[latCount][longCount][1] = y; //y
	      vertices[latCount][longCount][2] = z; //z
		}
	}
	for(int latCount=0; latCount<Latitudes-1; latCount++){
		glBegin(GL_QUADS);
	        for(int longCount=0; longCount<=Longitudes-1; longCount++){
				float *normal=calculaNormal(vertices[latCount][longCount],vertices[latCount][longCount+1],vertices[latCount+1][longCount]);
								glNormal3f(normal[0],normal[1],normal[2]);
				glVertex3f(vertices[latCount][longCount][0], vertices[latCount][longCount][1], vertices[latCount][longCount][2]);
				glNormal3f(normal[0],normal[1],normal[2]);
				glVertex3f(vertices[latCount][longCount+1][0],vertices[latCount][longCount+1][1],vertices[latCount][longCount+1][2]);
				glNormal3f(normal[0],normal[1],normal[2]);
				glVertex3f(vertices[latCount+1][longCount+1][0],vertices[latCount+1][longCount+1][1],vertices[latCount+1][longCount+1][2]);	
				glNormal3f(normal[0],normal[1],normal[2]);
	            glVertex3f(vertices[latCount+1][longCount][0], vertices[latCount+1][longCount][1], vertices[latCount+1][longCount][2]);
				glNormal3f(normal[0],normal[1],normal[2]);

				free(normal);
	        }
	   	 glEnd();
	}
} 

