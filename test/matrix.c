#include<stdio.h>
#include<stdlib.h>
#define M 100
int main(){
    int i,j,k,matrix1[M][M],matrix2[M][M],row1,col1,row2,col2,matrix[M][M];   
    printf("输入第一个矩阵的行数和列数："); 
    scanf("%d%d",&row1,&col1);
    printf("输入第一个矩阵：\n");
    for(i=0;i<row1;i++){
        for(j=0;j<col1;j++){
            scanf("%d",&matrix1[i][j]); 
        } 
    } 
    printf("输入第二个矩阵的行数和列数：");
    scanf("%d%d",&row2,&col2);
    printf("输入第二个矩阵：\n");
    for(i=0;i<row2;i++){
        for(j=0;j<col2;j++){
            scanf("%d",&matrix2[i][j]); 
        } 
    }
    for(i=0;i<row1;i++){
        for(j=0;j<col2;j++){
            matrix[i][j]=0; 
        } 
    } 
    if(col1!=row2){
        fprintf(stderr,"enput error!");
        exit(EXIT_FAILURE); 
    } 
    printf("The result:\n"); 
    for(i=0;i<row1;i++){
        for(j=0;j<col2;j++){
            for(k=0;k<col1;k++){
                matrix[i][j]=matrix[i][j]+matrix1[i][k]*matrix2[k][j]; 
            } 
        } 
    }
    for(i=0;i<row1;i++){
        for(j=0;j<col2;j++){
            printf("%d ",matrix[i][j]); 
        } 
        printf("\n"); 
    } 
    return 0;
}
