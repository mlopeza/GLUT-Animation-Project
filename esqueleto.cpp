#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


#define MAX_CHILD 5
#define MAX_ELEMENTCOUNT 20

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
//Librerias en Windows
#include <glut.h>
#else
//Librerias en Linux
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

//Headers Propios
#include "reader.h"
#include "lights.h"
#include "solido.h"

// Angulo de rotacion de la camara
float angle = 0.0f;

// Vector que representa la direccion de la camara
float lx=0.0f,lz=-1.0f, ly=-.5f;

// Posicion XZ de la camara
float x=0.0f, z=5.0f, y=1.0f;

//Cuando ninguna tecla se presiona
float deltaAngle = 0.0f;
float deltaMovez = 0;
float deltaMovey = 0;
int xOrigin = -1;

//nombre del elemento actual
char *currentName = NULL;

//Modo inicial = cabeza dice que SI
int modo = 1;

// Grados de libertad de los elementos
float minAng, maxAng;

//Matriz de Movimiento
Matriz *m;

//Quadric de luz
GLUquadricObj *spotQ;

//Manejo de materiales
GLfloat matAmbient [] =  {0.329412,0.223529,0.027451,
						  0.0215, 0.1745, 0.0215, 
						  0.135, 0.2225, 0.1575,
						  0.25,0.20725,0.20725,
						  0.2125,0.1275,0.054,
						  0.24725,0.1995,0.0745,
						  0.19225,0.19225,0.19225,
						  0.1,0.18725,0.1745};
GLfloat matDiffuse [] =  {0.780392,0.568627,0.113725,
						  0.07568, 0.61424, 0.07568, 
						  0.54, 0.89, 0.63,
						  1,0.829,0.829,
						  0.714,0.4284,0.18144,
						  0.75164,0.60648,0.22648,
						  0.50754,0.50754,0.50754,
						  0.396,0.74151,0.69102};
GLfloat matSpecular [] =  {0.992157, 0.941176,0.807843,
							.633, 0.727811, 0.633, 
		    				0.316228, 0.316228, 0.316228,
							.296648,0.296648,0.296648,
							0.393548,0.271906,0.166721,
							0.628281,0.555802,0.366065,
							0.508273,0.508273,0.508273,
							0.297254,0.30829,0.306678};
GLfloat matShininess [] =  {27.8974, 0.6, 0.1,0.088,0.2,0.4,0.4,0.1};
int matAsign;  //Asignación de material Actual

bool cambia = true; //Variable para manejar el cambio de material de Sombrero, Torso y Zapatos

GLfloat mat[4]; //Matriz auxiliar para los parametros de asignación de cada material

typedef struct _Elemento
{
	char name[20];		/* Nombre del Nodo*/
	char type[20];          /* Tipo de forma */
	GLfloat dimension[2];   /* Dimensiones alto, ancho */
	int theta[3];		/* Angulos x, y, z*/
	int limitsx[2];         /*Limites en x y z*/
	int limitsy[2];
	int limitsz[2];
	int childCount;         /* Numero de hijos */
	GLfloat trans_x;        /* Traslaciones en x y z*/
	GLfloat trans_y;
	GLfloat trans_z;
	GLUquadricObj *q;       /* Objeto quadric*/
	
	struct _Elemento *child[MAX_CHILD],	/* Hijos */
	*parent;				/* Padre */

} Elemento;
int dibuja(Elemento *nodo);

/* Nodo Raiz
 * Se agrego aqui debido a que se necesitan liberar
 * los apuntadores al finalizar el programa, ya que se debe llamar a una funcion
 * y no pasa parametros.
 * */
Elemento *raiz;

/* Funcion que Agrega un apuntador a un nodo.
 * Cada nodo puede tener un numero maximo de hijos que se puede ver en MAX_CHILD
 * La funcion regresa el nodo recien creado, aunque no se utiliza
 * ya que todo se hace por medio de referencia al nodo raiz y al nodo creado anteriormente
 * por la misma funcion.
 * */
Elemento *agregaElemento(Elemento **root,Elemento **temp,char *hijo, char *name,char *type, int theta[3],int limitsx[2],int limitsy[2],int limitsz[2],
		GLfloat dimension[2],GLfloat trans_x,GLfloat trans_y,GLfloat trans_z ){
	Elemento *t;
	int i;
	
	if(!(*root) && hijo[0]=='r'){
		//Crea el nodo raiz y define a su padre como NULL
		if(!(*root= (Elemento *)malloc(sizeof(Elemento))))
			return NULL;
		(*root)->parent=NULL;
		(*temp) = *root;
	
	}else if(hijo[0]=='h' && (*temp)->childCount < MAX_CHILD){
		if(!(t = (Elemento *)malloc(sizeof(Elemento))))
			return NULL;
		t->parent = *temp;
		(*temp)->child[(*temp)->childCount] = t;
		(*temp)->childCount++;
		(*temp)=t;

	}else if(hijo[0]=='x' && (*root)->childCount < MAX_CHILD){
		if(!(t = (Elemento *)malloc(sizeof(Elemento))))
			return NULL;
		(*temp) = (*root);
		t->parent = (*temp);
		(*temp)->child[(*temp)->childCount] = t;
		(*temp)->childCount++;
		(*temp)=t;	
	}else{
		//No se pudo agregar al hijo
		return NULL;
	}

	strcpy((*temp)->name,name);
	strcpy((*temp)->type,type);
	(*temp)->childCount = 0;
	(*temp)->theta[0] = theta[0];
	(*temp)->theta[1] = theta[1];
	(*temp)->theta[2] = theta[2];
	(*temp)->limitsx[0] = limitsx[0];
	(*temp)->limitsx[1] = limitsx[1];
	(*temp)->limitsy[0] = limitsy[0];
	(*temp)->limitsy[1] = limitsy[1];
	(*temp)->limitsz[0] = limitsz[0];
	(*temp)->limitsz[1] = limitsz[1];
	(*temp)->dimension[0] = dimension[0];
	(*temp)->dimension[1] = dimension[1];
	(*temp)->trans_x = trans_x;
	(*temp)->trans_y = trans_y;
	(*temp)->trans_z = trans_z;
	(*temp)->q = gluNewQuadric();
	gluQuadricNormals((*temp)->q, GLU_SMOOTH);
	for(i=0; i < MAX_CHILD;i++){
		(*temp)->child[i]= NULL;
	}
	return *temp;

}

/* Funcion que crea el arbol.
 * Cada nodo del arbol es leido desde un archivo de texto que le es pasado como parametro.
 * El nodo puede tener un maximo de MAX_CHILD hijos.
 * Todo se hace en base a el nodo raiz y al ultimo nodo creado.
 * El formato de lectura es:
 * hijo name type theta[0] theta[1] theta[2] limitsx[0] limitsx[1] limitsy[0] limitsy[1] limitsz[0] limitsz[1] dimension[0] dimension[1] trans_x trans_y trans_z trash
 *
 * Explicacion:
 * hijo: Es un caracter que define si el nodo se hace con respecto a la raiz o si es un hijo del nodo anterior. x(con raiz) (h) hijo [b(hermano) proximo]
 * name: Nombre del objeto, es un String sin espacios
 * type: [sphere|cube|cylinder] en minisculas, este define que tipo de objeto es.
 * theta[0]: angulo x 
 * theta[1]: angulo y
 * theta[2]: angulo z
 * limitsx[]: limites en x (El rango que puede tener de libertad el objeto para rotar)
 * limitsy[]: limites en y
 * limitsz[]: limites en z
 * dimension[0]: altura
 * dimension[1]: ancho
 * trans_x: traslacion en x
 * trans_y: traslacion en y
 * trans_z: traslacion en z
 * */
Elemento *cargaArbol(char *path){
	Elemento *root, *temp;

	FILE *file;

	GLfloat dimension[2];
	int theta[3];
        int limitsx[2];
	int limitsy[2];
	int limitsz[2];
	GLfloat trans_x;
	GLfloat trans_y;
	GLfloat trans_z;

	int unusedChildrenCount;
	char name[20],hijo[20],type[20],buffer[1024], trash[2];

	if (!(file = fopen(path, "r"))){
		fprintf(stderr, "No se pudo abrir el archivo %s \n", path);
		return NULL;
	}

	root = NULL;
	temp = NULL;
	while (!feof(file)){
		fgets(buffer, 1024, file);
		sscanf(buffer, "%s %s %s %d %d %d %d %d %d %d %d %d %f %f %f %f %f %[^\n]",
				hijo,name,type, 
				&theta[0], &theta[1], &theta[2],
				&limitsx[0],&limitsx[1],
				&limitsy[0],&limitsy[1],
				&limitsz[0],&limitsz[1], 
				&dimension[0], &dimension[1], 
				&trans_x, &trans_y, &trans_z,trash);

		/*Evita que se lea dos veces la ultima linea*/
		if(feof(file))
			continue;
		/* Ignora Strings Vacios */
		if (strlen(buffer) < 3)
			continue;
		//Agrega los elementos al arbol	
		agregaElemento(&root,&temp,hijo,name,type,theta,limitsx,limitsy,limitsz,dimension,trans_x,trans_y,trans_z);
	}

	return root;
}

/* Funcion seleccionaElemento que selecciona un
 * elemento del arbol para su movimiento (rotaciones)
 */
Elemento *seleccionaElemento(Elemento *root, const char *name){
	int i;
	Elemento *p;
	//Si no hay elementos
	if(!root)
		return NULL;
	//Checa el nombre del elemento actual
	if(!strcmp(root->name, name))
			return root;
	for(i = 0; i < root->childCount; i++)
	{
		//búsqueda recursiva
		p = seleccionaElemento(root->child[i], name);
		//Si se encuentra un elemento en este subárbol
		if(p)
			return p;
	}
	//No encuentra elemento
	return NULL;
}


/*Funcion eleListNames que lista los nombres
 * de los elementos. Ayuda a llevar el control del elemento actual
 */
void eleListNames(Elemento *root,char names[MAX_ELEMENTCOUNT][20])
{
	int i, present;
	if(!root)

		return;
	//Checar si este nombre ya está en la lista
	present = 0;
	for(i=0; (i < MAX_ELEMENTCOUNT) && (names[i][0] != '\0'); i++)
		if(!strcmp(names[i], root->name))
		{
			present = 1;
			break;
		}
	//Si no esta presente y aún hay espacio en la lista
		if(!present && (i < MAX_ELEMENTCOUNT))
		{
			strcpy(names[i], root->name);
			if(i + 1 < MAX_ELEMENTCOUNT)
				names[i + 1][0] = '\0';
		}
		//Llenar lista con los nombres de los sub arboles
		for(i = 0; i< root->childCount; i++)
			eleListNames(root->child[i], names);
}

/* Funcion que imprime la estructura del 
 * arbol a partir de un nodo raiz
 * */
int imprimeArbol(Elemento *root, int tabs){
	printf("Nodo: %s\n",root->name);
	int i,j;
	for(i =0; i < root->childCount;i++){
		for(j=0;j<tabs+1;j++)
			printf("\t");
		imprimeArbol(root->child[i], tabs + 1);
	}
	return 1;
}

/*
 * Libera los apuntadores creados
 * para la estructura del arbol
 * */
int liberaNodos(Elemento *root){
	int i;
	for(i =0; i < root->childCount;i++){
		liberaNodos(root->child[i]);
	}
//	printf("Se libero: %s \n",root->name);
	gluDeleteQuadric(root->q);
	free(root);
	return 0;
}

/*
 * Funcion utilizada para redimensionar
 * los objetos cuando se ha redimensionado
 * la pantalla
 * */
void changeSize(int w, int h) {
	// Se previene la dicion entre 0
	if (h == 0)
		h = 1;
	float ratio =  w * 1.0 / h;
	// MAtriz de proyeccion
	glMatrixMode(GL_PROJECTION);
	// Reset Matriz
	glLoadIdentity();
	// Viewport de la ventana
	glViewport(0, 0, w, h);
	// Perspectiva
	gluPerspective(45.0f, ratio, 0.1f, 100.0f);
	// Model View
	glMatrixMode(GL_MODELVIEW);
}


/* Calula el delta de los angulos de la camara
 *
 * */
void computePos(float deltaMove) {
	x += deltaMove * lx * 0.1f;
	z += deltaMove * lz * 0.1f;
}

void computeY(float deltaMove){
	y += deltaMove *ly * 0.1f;
}
/* Método asignaMaterial para cambiar el material del personaje
 *@i index del material asignado
 */
void asignaMaterial(int i){

	mat[0] = matAmbient[i];
    mat[1] = matAmbient[i+1];
    mat[2] = matAmbient[i+2];
    mat[3] = 1.0;
    glColor4fv(mat);
    mat[0] = matDiffuse[i];
    mat[1] = matDiffuse[i+1];
    mat[2] = matDiffuse[i+2];
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
    mat[0] = matSpecular[i];
    mat[1] = matSpecular[i+1];
    mat[2] = matSpecular[i+2];
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat);
    glMaterialf(GL_FRONT, GL_SHININESS, matShininess[i] * 128.0);

}
/*
 * Se encarga de dibujar la escena
 * */
void renderScene(void) {
	if (deltaMovez)
		computePos(deltaMovez);
	if (deltaMovey)
		computeY(deltaMovey);


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	// Posiciona la camara
	gluLookAt(	x, y, z,
			x+lx, y+ly,  z+lz,
			0.0f, 1.0f,  0.0f);
	//Seccion de Dibujo
	light0Pos[0]=x;
	light0Pos[1]=y-2;
	light0Pos[2]=z-12;

	dibuja(raiz);

	//Dibuja un solido de Revolucion
	glPushMatrix();
		glRotatef(seleccionaElemento(raiz, "Cuello")->theta[0], 1.0,0.0,0.0);
		glRotatef(raiz->theta[1], 0.0,1.0,0.0);
		glTranslatef(0.0,5.0,0.0);
		asignaMaterial((matAsign+1)%8);  //Asigna un material al sombrero
		drawRev();
	glPopMatrix();

	//Dibuja Media Esfera
	glPushMatrix();
		glTranslatef(0.0,5.5,0.0);
		glScalef(5.0,2.0,5.0);
		asignaMaterial((matAsign+1)%8);
		drawHS();
	glPopMatrix();

	//Dibuja una Esfera, donde se encuentra la Luz
	glPushMatrix();
		glTranslatef(x,y-2,z-13);
		glColor3f(1.0,1.0,1.0);
		gluSphere(spotQ,.2,10,10);
	glPopMatrix();
	glLightfv(GL_LIGHT0,GL_POSITION,light0Pos);


        glutSwapBuffers();
} 

/*
 * Dibuja los elementos de la estructura en forma recursiva
 * a partir del nodo raiz dado.
 * */
int dibuja(Elemento *nodo){
	//Asigna el material independiente de Torso y Pies.
	if((strcmp(nodo->name, "Torso") == 0) || (strcmp(nodo->name, "PI3") == 0) || (strcmp(nodo->name, "PD3") == 0)) {
		asignaMaterial((matAsign+1)%8);
	}
	else
		asignaMaterial(matAsign); //Asignacion de un material
	
	if(nodo == NULL)
		return 1;
	int i;
	glPushMatrix();
	glTranslatef(nodo->trans_x,nodo->trans_y,nodo->trans_z);
	glRotatef(nodo->theta[0],1.0,0.0,0.0);
	glRotatef(nodo->theta[1],0.0,1.0,0.0);
	glRotatef(nodo->theta[2],0.0,0.0,1.0);
	//Push
	//glColor3f(1.0,0.0,0.0);
	glPushMatrix();
	if(strcmp(nodo->type,"cube") == 0){
		glScalef(nodo->dimension[0],nodo->dimension[1],nodo->dimension[0]);
		glutSolidCube(1.0);
	}else if (strcmp(nodo->type,"cylinder") == 0){
		if(nodo->name[0]=='B'){
			glTranslatef(-(nodo->dimension[0]/2),0.0,0.0);
			glRotatef(90.0,0.0,1.0,0.0);
		}else{
			glTranslatef(0.0,-(nodo->dimension[0]/2),0.0);	
			glRotatef(-90.0,1.0,0.0,0.0);
		}
		gluCylinder(nodo->q,nodo->dimension[1],nodo->dimension[1],nodo->dimension[0],10,10);
	}else{
		gluSphere(nodo->q,(nodo->dimension[1]+nodo->dimension[0])/2,10,10);	
	}
	glPopMatrix();
	//FuncionRecursiva
	for(i=0; i<nodo->childCount;i++){
		dibuja(nodo->child[i]);
	}
	glPopMatrix();
 return 1;
}


/* Procesa las teclas presionadas
 *
 * */
void processNormalKeys(unsigned char key, int xx, int yy) { 
	Elemento *p;
 switch ( key ) {
	// cambia la rotación
	//Letra Y para rotar todo el personaje alrededor del eje Y
	case 'y':
		raiz->theta[1]+=5;
		break;
	//Letra u para rotar todo el personaje alrededor del eje Y
	case 'u':
		raiz->theta[1]-=5;
		break;
	//Letra i para cambiar el material asignado al personaje
	case 'i': matAsign += 1;
			  if(matAsign > 7) matAsign = 0;
		break;
	case 'h':
		break;
	case 'j':
		break;
	case 'k':    
		break;
   //Rotación de cada elemento de acuerdo a la opción seleccionada
	case 'q': switch(modo){
				 case 1: //Cabeza SI
						p = seleccionaElemento(raiz, "Cuello");
						if(p){
							maxAng = 45.0;
							p->theta[0] += (p->theta[0] < maxAng) ? 5 : 0;
						} 
		glutPostRedisplay();
				 break;
				 case 2: //Cabeza NO
						p = seleccionaElemento(raiz, "Cuello");
						if(p){
							maxAng = 85.0;
							p->theta[1] += (p->theta[1] < maxAng) ? 5 : 0;
						} 
				  break;
				  case 3: //Brazo Izq Completo en X theta[0]
						p = seleccionaElemento(raiz, "T->BI1");
						if(p){
							maxAng = 45.0;
							p->theta[0] += (p->theta[0] < maxAng) ? 5 : 0;
						} 
				  break;
				  case 4: //Brazo Izq Completo en Y theta[1]
						p = seleccionaElemento(raiz, "T->BI1");
						if(p){
							maxAng = 80.0;
							p->theta[1] += (p->theta[1] < maxAng) ? 5 : 0;
						} 
					break;
				 case 5: //Brazo Izq Completo en Z theta [2]
						p = seleccionaElemento(raiz, "T->BI1");
						if(p){
							maxAng = 90.0;
							p->theta[2] += (p->theta[2] < maxAng) ? 5 : 0;
						}
				  break;
				  case 6: //Codo Izquierdo en Y
						p = seleccionaElemento(raiz, "BI1->BI2");
						if(p){
							maxAng = 160.0;
							p->theta[1] += (p->theta[1] < maxAng) ? 5 : 0;
						}
					break;
				 case 7: //Mano izquierda en Z
						p = seleccionaElemento(raiz, "BI2->BI3");
						if(p){
							maxAng = 70.0;
							p->theta[2] += (p->theta[2] < maxAng) ? 5 : 0;
						}
				  break;
				  //Brazo derecho completo en X
				 case 8: 
		    				p = seleccionaElemento(raiz, "T->BD1");
				   		if(p){
				     			maxAng = 45.0;
				     			p->theta[0] += (p->theta[0] < maxAng)? 5:0;
				   		}
				 break;
				 //Brazo derecho completo en y
				 case 9: 
				   		p= seleccionaElemento(raiz,"T->BD1");
				   		if(p){
				     			maxAng=-80.0;
				     			p->theta[1] -= (p->theta[1] >  maxAng)? 5:0;
				   		}
				   break;
		                 case 10: //Brazo derecho completo en z
				   		p = seleccionaElemento(raiz,"T->BD1");
				   		if(p){
				     			maxAng=90;
				       			p->theta[2] += (p->theta[2] < maxAng)? 5:0;
				   		}
				   break;
				 case 11: //Codo derecho 
						p = seleccionaElemento(raiz,"BD1->BD2");
						if(p){
							maxAng = -160;
							p->theta[1] -=(p->theta[1] > maxAng) ? 5:0;
						}
				   break;
				 case 12: //Mano derecha
						p = seleccionaElemento(raiz, "BD2->BD3");
						if(p){
							maxAng = -70.0;
							p->theta[2] -= (p->theta[2] > maxAng) ? 5 : 0;
						}
				   break;
				 case 13: //Pierna Izquierda completa en X
						p = seleccionaElemento(raiz, "T->PI1");
						if(p){
							maxAng = 60.0;
							p->theta[0] += (p->theta[0] < maxAng) ? 5 : 0;
						}
				  break;
				  case 14:  //Pierna Izquierda completa en Z
						p = seleccionaElemento(raiz, "T->PI1");
						if(p){
							maxAng = 0.0;
							p->theta[2] += (p->theta[2] < maxAng) ? 5 : 0;
						}
				  break;
				  case 15:  //Rodilla solo en X
						p = seleccionaElemento(raiz, "PI1->PI2");
						if(p){
							maxAng = 45.0;
							p->theta[0] += (p->theta[0] < maxAng) ? 5 : 0;
						}
				  break;
				  case 16: //Pie Izquierdo solo en Y
						p = seleccionaElemento(raiz, "PI2->PI3");
						if(p){
							maxAng = 0.0;
							p->theta[1] += (p->theta[1] < maxAng) ? 5 : 0;
						}
				  break;
				  
				  case 17: //Pierna Derecha en X	
						p= seleccionaElemento(raiz, "T->PD1");
						if(p){
							maxAng = 60.0;
							p->theta[0] += (p->theta[0] < maxAng) ? 5: 0;	
						}		
				  break;
				  case 18: //Pierna Derecha en Z						
						p = seleccionaElemento(raiz, "T->PD1");
						if(p){
						     	maxAng = 0.0;
							p->theta[2] -= (p->theta[2] > maxAng) ? 5:0;
						}
				  break;
				  case 19: //Rodilla en X
						p=seleccionaElemento(raiz,"PD1->PD2");
						if(p){
							maxAng = 45.0;
							p->theta[0] += (p->theta[0] < maxAng)? 5:0;
						}
				  break;
				  case 20: //Pie derecho
						p=seleccionaElemento(raiz,"PD2->PD3");
						if(p){
							maxAng = 0.0;
							p->theta[1] -= (p->theta[1]>maxAng)? 5:0;
						}
				  break;
				}
		break;
	case 'w':  switch(modo){
				 case 1:  
						p = seleccionaElemento(raiz, "Cuello");
						if(p){
							minAng = -30.0;
							p->theta[0] -= (p->theta[0] > minAng) ? 5 : 0;
						} 
				 break;
				 case 2: 
						p = seleccionaElemento(raiz, "Cuello");
						if(p){
							minAng = -85.0;
							p->theta[1] -= (p->theta[1] > minAng) ? 5 : 0;
						} 
				  break;
				 case 3:
					 p = seleccionaElemento(raiz, "T->BI1");
						if(p){
							minAng = -90.0;
							p->theta[0] -= (p->theta[0] > minAng) ? 5 : 0;
						} 
				 break;
				 case 4:
					 p = seleccionaElemento(raiz, "T->BI1");
						if(p){
							minAng = -70.0;
							p->theta[1] -= (p->theta[1] > minAng) ? 5 : 0;
						} 
				 break;
				 case 5:
					 p = seleccionaElemento(raiz, "T->BI1");
						if(p){
							minAng = -90.0;
							p->theta[2] -= (p->theta[2] > minAng) ? 5 : 0;
						} 
				 break;
				 case 6:
					 p = seleccionaElemento(raiz, "BI1->BI2");
						if(p){
							minAng = 0.0;
							p->theta[1] -= (p->theta[1] > minAng) ? 5 : 0;
						} 
				 break;
				 case 7:
					 p = seleccionaElemento(raiz, "BI2->BI3");
						if(p){
							minAng = -70.0;
							p->theta[2] -= (p->theta[2] > minAng) ? 5 : 0;
						} 
				 break;
				   //Brazo derecho completo en X
				 case 8: 
		    				p = seleccionaElemento(raiz, "T->BD1");
				   		if(p){
				     			maxAng = -30.0;
				     			p->theta[0] -= (p->theta[0] > maxAng)? 5:0;
				   		}
				 break;
				 //Brazo derecho completo en y
				 case 9: 
				   		p=seleccionaElemento(raiz,"T->BD1");
				   		if(p){
				     			maxAng=80.0;
				     			p->theta[1] += (p->theta[1] <  maxAng)? 5:0;
				   		}
				   break;
		                 case 10: //Brazo derecho completo en z
				   		p = seleccionaElemento(raiz,"T->BD1");
				   		if(p){
				     			maxAng=-90;
				       			p->theta[2] -= (p->theta[2] > maxAng)? 5:0;
				   		}
				   break;
				 case 11: //Codo derecho 
						p = seleccionaElemento(raiz,"BD1->BD2");
						if(p){
							maxAng = 0;
							p->theta[1] +=(p->theta[1] < maxAng) ? 5:0;
						}
				   break;
				 case 12: //Mano derecha
						p = seleccionaElemento(raiz, "BD2->BD3");
						if(p){
							maxAng = +70.0;
							p->theta[2] += (p->theta[2] < maxAng) ? 5 : 0;
						}
				   break;
				 case 13: p = seleccionaElemento(raiz, "T->PI1");
						if(p){
							minAng = -80.0;
							p->theta[0] -= (p->theta[0] > minAng) ? 5 : 0;
						} 
				 break;
				 case 14: p = seleccionaElemento(raiz, "T->PI1");
						if(p){
							minAng = -45.0;
							p->theta[2] -= (p->theta[2] > minAng) ? 5 : 0;
						} 
				 break;
				 case 15: p = seleccionaElemento(raiz, "PI1->PI2");
						if(p){
							minAng = 0.0;
							p->theta[0] -= (p->theta[0] > minAng) ? 5 : 0;
						} 
				 break;
				 case 16: p = seleccionaElemento(raiz, "PI2->PI3");
						if(p){
							minAng = -40.0;
							p->theta[1] -= (p->theta[1] > minAng) ? 5 : 0;
						} 
				 break;
				  case 17: //Pierna Derecha en X	
						p= seleccionaElemento(raiz, "T->PD1");
						if(p){
							maxAng = -60.0;
							p->theta[0] -= (p->theta[0] > maxAng) ? 5: 0;	
						}		
				  break;
				  case 18: //Pierna Derecha en Z						
						p = seleccionaElemento(raiz, "T->PD1");
						if(p){
						     	maxAng = +45.0;
							p->theta[2] += (p->theta[2] < maxAng) ? 5:0;
						}
				  break;
				  case 19: //Rodilla en X
						p=seleccionaElemento(raiz,"PD1->PD2");
						if(p){
							maxAng = 0.0;
							p->theta[0] -= (p->theta[0] > maxAng)? 5:0;
						}
				  break;
				  case 20: //Pie derecho
						p=seleccionaElemento(raiz,"PD2->PD3");
						if(p){
							maxAng = 50.0;
							p->theta[1] += (p->theta[1]<maxAng)? 5:0;
						}
				  break;
				}
		break;
	default: break;
 }
} 


/* Procesa las teclas presionadas (Teclas especiales)
 *
 * */
void pressKey(int key, int xx, int yy) {
	   Elemento *i, *d;
       switch (key) {
         case GLUT_KEY_UP : deltaMovez = 0.5f; break;
         case GLUT_KEY_DOWN : deltaMovez = -0.5f; break;
	     case GLUT_KEY_RIGHT : deltaMovey = -0.5f;break;
	     case GLUT_KEY_LEFT : deltaMovey = 0.5f;break;
       }
} 

/* Captura el evento de soltar una tecla.
 *
 * */
void releaseKey(int key, int x, int y) { 	

        switch (key) {
             case GLUT_KEY_UP:
             case GLUT_KEY_DOWN : deltaMovez = 0;break;
             case GLUT_KEY_RIGHT :
			 case GLUT_KEY_LEFT :deltaMovey = 0;break;
        }
} 

/* Captura el evento de mover el raton
 * (Se utiliza para mover la camara en x)
 *
 * */
void mouseMove(int x, int y) { 	

         // Boton izquierdo
         if (xOrigin >= 0) {

		// actualiza deltaAngle
		deltaAngle = (x - xOrigin) * 0.001f;

		// Actualiza la direccion de la camara
		lx = sin(angle + deltaAngle);
		lz = -cos(angle + deltaAngle);
	}
}


/* Captura el evento de un click
 *
 * */
void mouseButton(int button, int state, int x, int y) {

	// Solo funciona si el boton izquierdo se presiono
	if (button == GLUT_LEFT_BUTTON) {
		// Cuando el boton se suelta
		if (state == GLUT_UP) {
			angle += deltaAngle;
			xOrigin = -1;
		}
		else  {//Cuando esta presionado
			xOrigin = x;
		}
	}

}
//Funcion funcionMenu
void funcionMenu(int valor){
	if(valor == 100)  //Selecciona la opción Salir para terminar el programa
		exit(0);
}
/* Funcion subMenu para el procesamiento de las opciones
 * del submenú
 */
void funcionSubMenu(int valor){
	modo = valor;
	glutPostRedisplay();
}

void funcionSubMenu2(int valor){
	modo = valor;
	glutPostRedisplay();
}
/*Función AgregaMenú que se encarga de agregar el
 * menú para iniciar a mover algún elemento del personaje
 */
void agregaMenu(){
	int menu, submenu, submenu2;
	int subBrazoDerecho,subBrazoIzquierdo;
	int subPiernaDerecha,subPiernaIzquierda;
	int subCabeza;
	
	subBrazoDerecho = glutCreateMenu(funcionSubMenu);
	glutAddMenuEntry("Hombro Derecho X",8);
	glutAddMenuEntry("Hombro Derecho Y",9);
	glutAddMenuEntry("Hombro Derecho Z",10);
	glutAddMenuEntry("Codo Derecho Y",11);
	glutAddMenuEntry("Mano Derecha",12);

	subBrazoIzquierdo = glutCreateMenu(funcionSubMenu);
	glutAddMenuEntry("Hombro Izquierdo X",3);
	glutAddMenuEntry("Hombro Izquierdo Y",4);
	glutAddMenuEntry("Hombro Izquierdo Z",5);
	glutAddMenuEntry("Codo Izquierdo Y",6);
	glutAddMenuEntry("Mano Izquierda",7);

	subPiernaDerecha = glutCreateMenu(funcionSubMenu);
	glutAddMenuEntry("Pierna Derecha X",17);
	glutAddMenuEntry("Pierna Derecha Z",18);
	glutAddMenuEntry("Rodilla Derecha X",19);
	glutAddMenuEntry("Pie Derecho",20);

	subPiernaIzquierda = glutCreateMenu(funcionSubMenu);
	glutAddMenuEntry("Pierna Izquierda X",13);
	glutAddMenuEntry("Pierna Izquierda Z",14);
	glutAddMenuEntry("Rodilla Izquierda X",15);
	glutAddMenuEntry("Pie Izquierdo",16);

	subCabeza = glutCreateMenu(funcionSubMenu);
	glutAddMenuEntry("Si", 1);
	glutAddMenuEntry("No", 2);


	menu= glutCreateMenu(funcionMenu);
	submenu = glutCreateMenu(funcionSubMenu);
	glutSetMenu(menu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutAddMenuEntry("Salir", 100);
	glutAddSubMenu("Rotaciones",submenu);
	glutSetMenu(submenu);
	glutAddSubMenu("Brazo Derecho",subBrazoDerecho);
	glutAddSubMenu("Brazo Izquierdo",subBrazoIzquierdo);
	glutAddSubMenu("Pierna Derecha",subPiernaDerecha);
	glutAddSubMenu("Pierna Izquierda",subPiernaIzquierda);
	glutAddSubMenu("Cabeza",subCabeza);
}

/* Funcion auxiliar para mandar llamar a la funcionq ue libera
 * la memoria que fue reservada de forma dinamica.
 * */
void liberaAux(){
	liberaNodos(raiz);
}

int main(int argc,char **argv){
	//Inicializar arreglo de nombres
	char names[MAX_ELEMENTCOUNT][20];
	names [0][0] = '\0';
	int nameIndex = 0;
	eleListNames(raiz, names);
	
	matAsign = 0; //Asignacion de primer material
	int i;
	for(i = 0; (i < MAX_ELEMENTCOUNT) && (names[i][0] != '\0'); i++)
		printf("nombre elemento: %s\n", names[i]);

	currentName = names [nameIndex];

	//Inicia Glut
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	//Inicializa spot
	spotQ=gluNewQuadric();
	//GLUT Window	
	glutInitWindowSize(500,500);
	glutCreateWindow("Personaje");
	init();
	//Funciones de Dibujo
	computePos(-100);
	computeY(-100);
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	glutIdleFunc(renderScene);

	//Funciones de teclado
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(pressKey);
	glutSpecialUpFunc(releaseKey);
	//Funciones del mouse
	glutMouseFunc(mouseButton);

	glutMotionFunc(mouseMove);
	//Agrega menu
	agregaMenu();
	//Open GL init
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE); 
	//Grueso de las lineas a dibujar
	glLineWidth(3.0); 
	//Se carga la estructura de datos
	raiz = cargaArbol("sk.txt");
	
	//Funcion que libera los nodos
	atexit(liberaAux);
	//Inicia el Loop de GLUT
	glutMainLoop();
}
