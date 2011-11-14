

int lights; //se define la variable luces 1.0,2.0,3.0,1.0f es valor para ubicar al primer cubo
GLfloat	 light0Pos[] = { 1.0, 2.0, 3.0, 1.0f }; //nuevo -1,3,-0.5,1.0f (1.0f es el azul original y -1,0f es azul oscuro)

void init(void){
	
	GLfloat  ambientLight[] =  { 0.1f, 0.1f, 0.1f, 1.0f };//muy oscuro
	GLfloat  ambient0Light[] = { 0.1f, 0.1f, 0.1f, 1.0f };//oscuro = valores menores
	GLfloat  diffuse0Light[] = { 0.5f, 0.5f, 0.5f, 1.0f };//medio
	GLfloat  specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };//claro
	GLfloat  specular0Light[] ={ 1.0f, 1.0f, 1.0f, 1.0f };//claro

	// habilita ambas caras (FRONT-iluminada & BACK-oscurecida) de los poligonos
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
	// luz ambiental de toda la escena
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight );

	// habilita el color para los materiales (poligonos rellenos)
	glEnable(GL_COLOR_MATERIAL);
	// luz Specular para todos los poligonos
	glMaterialfv(GL_FRONT, GL_SPECULAR, specularLight );
	// brillantez del objeto para la luz Specular (0-128)
	glMateriali(GL_FRONT,GL_SHININESS,100); //entre más grande es menos brillante

    // define la LUZ-0
	glLightfv(GL_LIGHT0,GL_AMBIENT,ambient0Light);
	//glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse0Light);
	glLightfv(GL_LIGHT0,GL_SPECULAR,specular0Light);
	// la ubicacion realmente se hara en el momento de dibujar
	//glLightfv(GL_LIGHT0,GL_POSITION,light0Pos);

	///////////////////////////////////
	
	glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION,light0Pos); //dirección del foco

	glLightf(GL_LIGHT0,GL_SPOT_CUTOFF,100); //Apertura del foco, tercer valor indica los grados

	glLightf(GL_LIGHT0,GL_SPOT_EXPONENT,100); //Atenuación del foco, a medida que nos acercamos a él

	// habilita todas las luces
	glEnable(GL_LIGHTING);
	// habilita la LUZ-0
	glEnable(GL_LIGHT0);
	//Manejo de color
	 glColorMaterial(GL_FRONT, GL_DIFFUSE);
     glEnable(GL_COLOR_MATERIAL);
	
	lights=1;

}




