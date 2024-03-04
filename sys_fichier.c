#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

// Rafik BENNACER : rafik.bennacer@universite-paris-saclay.fr


// Sucre syntaxique 
#define AND &&
#define OR ||
#define ISNOT !=
#define NOT !


typedef enum { false , true  } bool ; 

// predeclarations 
int file ( char *source );
int repertoire ( char *src ) ;
int copyfile1 ( char *source , char *dest );
int copyfile ( char *source , char *dest );
char *dir (char *src , char*dest);




// Solutions : 

// retourne 1 ou 0 si l'argument est un fichier ou pas (repertoire ...)
int file ( char *source ) {
    struct stat s  ; 
    stat (source , &s ) ; 
    return S_ISREG(s.st_mode) ; 
}



// Question 1 


int copyfile1 ( char *source , char *dest ) {


    if ( NOT(file(source )) ){
        perror ("Erreur : la source n'est pas un fichier.\n ") ; 
    }

    if ( NOT(file ( dest )) ) {
        perror ("Erreur : la destination n'est pas un fichier. \n") ;
    }

    int src = open (source , O_RDONLY) ; 
    int dst = open (dest , O_WRONLY | O_CREAT | O_TRUNC) ; 

    if (src == -1 ) {
        perror ("Erreur ouverture fichier source. \n ") ; 
        return -1 ; 
    }
    if (dst == -1 ) {
        perror ("Erreur ouverture fichier destination . \n ") ; 
        return -1 ; 
    }

    int p = 0 ; 
    do{
        char buffer [4096]; 
        int r =read ( src , buffer , sizeof(buffer )) ; 
        if (r == 0 ) break ; // on sort de la boucle et on fini de lire le fichier jusqu'a la fin 
        while ( r ISNOT 0 ) { 
            int w = write (dst , buffer + p , src  ) ; 
            r = r - w ; 
            p = p + w ; // on met a jout la mositions dans le fichier 

        } 

    } while (true) ; 

    close (src ) ; 
    close (dst) ; 

return 0 ; 
    
}





int copyfile ( char *source , char *dest ) {


    if ( NOT(file(source ))){
        perror ("Erreur : la source n'est pas un fichier.\n ") ; 
    }

    if ( NOT(file ( dest ))) {
        perror ("Erreur : la destination n'est pas un fichier. \n") ;
    } 

    int src = open (source , O_RDONLY) ; 
    int dst = open (dest , O_WRONLY | O_CREAT | O_TRUNC) ; 
    

    if (src == -1 ) {
        perror ("Erreur ouverture fichier source. \n ") ; 
        return -1 ; 
    }
    if (dst == -1 ) {
        perror ("Erreur ouverture fichier destination . \n ") ; 
        return -1 ; 
    } 

    // Changement de droit 

    struct stat filestats ; 
    fstat (src  , &filestats) ; 
    fchmod (dst , filestats.st_mode ) ;


    int p = 0 ; 
    // Copie du fichier
    do {
        char buffer [4096]; 
        int r =read ( src , buffer , sizeof(buffer )) ; 
        if (r == 0 ) break ; // on sort de la boucle et on fini de lire le fichier jusqu'a la fin 
        while ( r ISNOT 0 ) { 
            int w = write (dst , buffer + p , r ) ; 
            r = r - w ; 
            p = p + w ; // on met a jout la mositions dans le fichier 

        }

    } while(true) ; 

    close (src ) ; 
    close (dst) ; 

return 0 ; 
    
}


// Question 3 

// Repertoire (1 ou 0 )

int repertoire ( char *src ) {
    struct stat s ; 
    stat (src , &s) ; 
    return S_ISDIR(s.st_mode) ; 
}


// creation du chemain
char *dir (char *src , char*dest) {
    if (src == NULL OR dest ==NULL ) return NULL ; 
    char *acc = malloc ( strlen(src) + strlen (dest ) + 2 ) ; 
    strcpy (acc , src) ;
    if ( src [ strlen(src) - 1 ] ISNOT '/') strcat (acc , "/") ; 
    strcat (acc , dest );
    return acc ; 

}





void copydirectory ( char *src , char *dst) {

    // Verif 
    if (NOT(repertoire(src))) {
        perror ("Erreur : ce n'est pas un repertoire.\n") ; 
        return ; 
    }

    if (NOT(repertoire(dst))) {
        perror("Erreur : ce n'est pas un repertoire.\n") ;
        return;  
    } 


    DIR *d = opendir(src) ; 


    // Changement de droit 

    struct stat s ; 
    stat ( src , &s); 
    mkdir ( src , s.st_mode) ; 
    chmod (dst , s.st_mode); 
    while (true ) {
        struct dirent *ent = readdir (d ) ; 
        if (ent == NULL) break  ; 
        if (ent ->d_name[0] == '.')
        continue ; 
        char *isrc = dir(src , ent->d_name); 
        char *idest = dir (dst , ent-> d_name); 

        copyfile (isrc , idest ) ;  // copie du fichier

        free (isrc) ; 
        free(idest); 



    }
    closedir (d) ; 



}

// Question 4

void copyDirectoryRecursive ( char *src , char *dst ) {

    if (NOT(repertoire(src))) {
        perror(" Erreur la source n'est un répertoire.\n");
        return;
    }

    if (NOT(repertoire(dst))) {
        perror(" Erreur la source n'est un répertoire.\n");
        return;
    }

    // Open the source directory
    DIR *d = opendir(src);
    if (d == NULL) {
        perror("Erreur.\n");
        return;
    }

    // Changements de droit
    struct stat s;
    stat(src, &s);
    mkdir(dst, s.st_mode);

    struct dirent *ent;
    while ((ent = readdir(d)) != NULL) {
        if (ent->d_name[0] == '.') continue;

        char *srcPath = dir(src, ent->d_name);
        char *dstPath = dir(dst, ent->d_name);

        stat(srcPath, &s);
        if (S_ISDIR(s.st_mode)) {
            
            copyDirectoryRecursive(srcPath, dstPath); 
        } else {
            
            copyfile(srcPath, dstPath); // copie du fichier 
        }

        free(srcPath);
        free(dstPath);
    }

    closedir(d);
}

int main (int argc , char *argv []) {


  /* char *src = "test.txt" ; 
    char *dest = "dest.txt" ; 
    copyfile(src , dest ) ; 
    */

   copyDirectoryRecursive("test","dest");
    return 0;

}

