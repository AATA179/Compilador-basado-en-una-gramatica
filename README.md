# Compilador-basado-en-una-gramatica
Se traduce la operación ingresada a lenguaje intermedio. Detectando los posibles errores, esto en base a la gramática:  
<E> → <E> + <T>
<E> → <E> - <T>
<E> → <T>                   
<T> → <T> * <T>  
<T> → <T> / <T>  
<T> → <U>                  
<U> → - <U>
<U> →( <E> )  
<U> → num          
