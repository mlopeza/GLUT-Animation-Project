#define LINES 3000.0 /* Maximod e lineas a dibujar */
#define SIZE 3 		/* Numero de puntos del vector de puntos */
/* Estructura de Punto*/ 
struct point3D{ 
	float x; 
	float y; 
	float z; 
}; 

/* Silueta de un sombrero */ 
struct point3D v[SIZE] = {
	{0.0,0.0,0.0},
	{1.5,-1.0,0.0},
	{1.8, -0.7, 0.0}
};

/* Dibuja silueta*/ 
void drawOutline() { 
	//Toma los puntos de la estructura y los dibuja en el angulo en el que se encuentra.
	int i; 
	glBegin(GL_LINE_STRIP); 
	for( i=0; i<SIZE; ++i){
		glVertex3f( v[i].x, v[i].y, v[i].z ); 
	} 
	glEnd(); 

} 

/* Revoluciona la silueta */ 
void drawRev(){
	//Se dibuja un solido de revolucion no conectado
	int i; 
	for( i=0;i<LINES;++i) { 
		//Recorrera los 360 Grados Divididos en el numero de Lineas que se desea dibujar.
		float frac = (float)i / LINES;
		glRotatef(frac*360,0.0,1.0,0.0); 
		drawOutline(); 
	} 
} 

/*
	Se calcula la normal de un Plano por medio de tres puntos
	v1 = p2 - p1
	v2 = p3 - p1
	v3 = v1 x v2
	Length es el tamaño del vector.
	v3 se divide y es el vector unitario es lo que se regresa.	
*/
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

	//Producto Cruz
	v3[0]=v1[1]*v2[2]-v1[2]*v2[1];
	v3[1]=v1[2]*v2[0]-v1[0]*v2[2];
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

	//Se calculan cada una de las coordenadas de la media esfera
	// Se divide en latitudes y longitudes
	for(int latCount=0; latCount<Latitudes; latCount++){ 
		for(int longCount=0; longCount<Longitudes; longCount++){ // x and y
			// Se utilizan coordenadas polares.
			// Se va haciendo un barrido en z calculando primero esta para calcular
			// r que es anguloq ue define a x y y,
			// Despues simplemente se calcula x y y
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
			//Se calcula la Normal
			float *normal=calculaNormal(vertices[latCount][longCount],vertices[latCount][longCount+1],vertices[latCount+1][longCount]);
			//Se dibujan cada uno de los vertices y se asigna una normal por cada vertice.
			//Se dibuja con la regla de la mano derecha.
			glNormal3f(normal[0],normal[1],normal[2]);
			glVertex3f(vertices[latCount][longCount][0], vertices[latCount][longCount][1], vertices[latCount][longCount][2]);
			glNormal3f(normal[0],normal[1],normal[2]);
			glVertex3f(vertices[latCount][longCount+1][0],vertices[latCount][longCount+1][1],vertices[latCount][longCount+1][2]);
			glNormal3f(normal[0],normal[1],normal[2]);
			glVertex3f(vertices[latCount+1][longCount+1][0],vertices[latCount+1][longCount+1][1],vertices[latCount+1][longCount+1][2]);	
			glNormal3f(normal[0],normal[1],normal[2]);
			glVertex3f(vertices[latCount+1][longCount][0], vertices[latCount+1][longCount][1], vertices[latCount+1][longCount][2]);
			glNormal3f(normal[0],normal[1],normal[2]);

			//Se libera el apuntador de la normal.
			free(normal);
		}
		glEnd();
	}
} 

