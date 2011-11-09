#define LINES 300.0 /* MAX lines to draw*/
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
	int i; 
	for( i=0;i<LINES;++i) { 
		float frac = (float)i / LINES; /* Recorrera los 360 Grados Divididos en el numero de Lineas que se desea dibujar.*/ 
		glRotatef(frac*360,0.0,1.0,0.0); 
		glColor3f(1.0-frac,0.0,frac); /*Cambia el color segun lalinea que se desea dibujar*/
		drawOutline(); 
	} 
} 


