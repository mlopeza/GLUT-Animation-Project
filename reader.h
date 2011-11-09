typedef struct _Matriz{
	int **matriz;
	int *size;
	int *pos;
	int *dir;
} Matriz;

Matriz *cargaThetas(char *path){
	FILE *file;
	Matriz *m;
	if(!(m = (Matriz *)malloc(sizeof(Matriz))))
		return NULL;
	
	int length;
	char buffer[1024],trash[2];

	if(!(file = fopen(path,"r"))){
		fprintf(stderr,"No se pudo abrir el archivo %s \n",path);
		return NULL;
	}

	fgets(buffer,1024,file);
	sscanf(buffer,"%d %[^\n]",&length,trash);
	m->matriz = new int* [length];
	m->pos=new int;
	*(m->pos)=0;
	m->size=new int;
	*(m->size)=length;
	m->dir=new int;
	*(m->dir)=1;
	for (int i=0;i<length;i++){
		m->matriz[i] = new int[3];
	}
	
	for(int i=0;i<length;i++){
		fgets(buffer,1024,file);
		sscanf(buffer,"%d %d %d %[^\n]",&(m->matriz[i][0]),&(m->matriz[i][1]),&(m->matriz[i][2]),trash);
		
	}
	return m;
}
