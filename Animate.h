int speed = 200;
char *animatedNodes[13];
int numAnimatedNodes;
int keyFrames;
int aniMatrix[100][13][3];
int animateMatrix = 0;
int direccion = 1;
void animateModel(int id){
	if(id == keyFrames-1){
		direccion = -1;
	}

	if(id == -1){
		direccion = 1;
		animateMatrix = 0;
		return;
	}

	Elemento *nodo;	
	for(int i=0;i<numAnimatedNodes;i++){
		nodo = seleccionaElemento(raiz,animatedNodes[i]);
		if(i == 1)
		printf("%s %d %d %d\n",animatedNodes[i],aniMatrix[id][i][0],aniMatrix[id][i][1],aniMatrix[id][i][2]);
		nodo->theta[0] = aniMatrix[id][i][0];
		nodo->theta[1] = aniMatrix[id][i][1];
		nodo->theta[2] = aniMatrix[id][i][2];
	
	}
	glutTimerFunc(speed,animateModel,id+direccion);

}


void changeColor(int id){
	matAsign = ((matAsign + 1) == 7)?0:matAsign+1;
	texAct = (texAct == 1)? 2 : 1;
	glutTimerFunc(3000,changeColor,0);
	return;
}

void animation(int id){
	

	glutTimerFunc(500,animation,0);
}

void leeAnimacion(char *direccion){
	
	printf("Hola\n");
	char buffer[1024],trash[2];

	FILE *file;
	if(!(file = fopen(direccion,"r"))){
		fprintf(stderr,"No se pudo abrir el archivo %s\n",direccion);
		return;
	}

	while(!feof(file)){
		fgets(buffer,1024,file);
		sscanf(buffer,"%d %d",&numAnimatedNodes,&keyFrames);
		printf("Buffer 6: %d %d",numAnimatedNodes,keyFrames);
		
		
		for(int i =0; i<numAnimatedNodes && !feof(file);i++){
			animatedNodes[i]=(char *) malloc(6*sizeof(char));
			fgets(buffer,1024,file);
			sscanf(buffer,"%s [^\n]",animatedNodes[i]);
		}


		int x = 0;
		for(int i =0;i<numAnimatedNodes;i++){
			for(int j=0;j<keyFrames;j++){
				fgets(buffer,1024,file);
				sscanf(buffer,"%d %d %d [^\n]",&aniMatrix[j][i][0],&aniMatrix[j][i][1],&aniMatrix[j][i][2]);
				x++;
			}
		
		}
		printf("X:%d\n",x);
		break;
	}
	for(int i =0;i<numAnimatedNodes;i++){
		for(int j=1;j<keyFrames;j++){
			printf("%d %d %d\n",aniMatrix[j][i][0],aniMatrix[j][i][1],aniMatrix[j][i][2]);
		}
	}
	printf("%d\n",keyFrames);
	animateMatrix = 1;
	glutTimerFunc(1000,animateModel,0);		

}




