/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package ring.based.v1;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author SebAs
 * Version 1
 */
public class Satellite extends Thread{
    private int id, ntask, nsat, enMax, sMax, nTasksAssig;
    private long Tstart, Tend;
    private double idTable[], auxTable[],comTable[][];   
    private AuxClass aux;
    private Task task;
    private Satellite nextSat;
    private ArrayList<Integer> tasksAssigned;
    private boolean finalVotes;
    
    public Satellite(int id, int ntask, int nsat, AuxClass aux, Task task){
        this.id = id;
        this.ntask = ntask; 
        this.nsat = nsat;
        idTable = new double[ntask];
        auxTable = new double[ntask];
        comTable = new double[ntask][2];
        this.task = task;
        this.aux = aux;
        finalVotes = false;
        tasksAssigned = new ArrayList();
    }
       
    public void setNeighbour(Satellite nextSat){
        this.nextSat = nextSat;
    }
    
    public void getVotes(){                  
       idTable = task.getVotes(enMax,sMax,id);              
    }   
    
    public void checkVotes(){
        if(!aux.votesChanged()){
            finalVotes = true;
        }       
    }   
    
    public void vote(){
        int index = 0;
        int tmp = 0;
        //get the maximum 
        double max = 0;       
        for (int k=0; k<ntask; k++){
            if(auxTable[k] > max){
                max = auxTable[k];
                index = k;
            }
        }       
        while(tmp < ntask){          
            if(comTable[index][0] < max){                
                comTable[index][0] = max;
                comTable[index][1] = id;
                aux.setChangeTo(true,id);
                break;
            } else if ((comTable[index][0] == max) && (comTable[index][1] == id)){
                aux.setChangeTo(false,id);
                break;
            } else {
                auxTable[index] = 0;               
                max=0;           
                for (int j=0; j<ntask; j++){
                    if((auxTable[j] > max)){
                        max = auxTable[j];
                        index = j;
                    }
                }
                if(max == 0){
                    aux.setChangeTo(false,id);
                    break;
                }               
            }
            tmp++;  
        }                    
    }
    
    public void giveTable(double[][] t) {                 
        comTable = t.clone();
    }    
    
    @Override
    public synchronized void run(){        
        Tstart = System.currentTimeMillis();              
        try {         
            while(true){
                while(aux.getTurn()!= id){
                    aux.waitSat();                
                }
                if(aux.getIt() == 1){
                    enMax = task.getEnergyCapacity();
                    sMax = task.getStorageCapacity();
                    getVotes();
                    auxTable = idTable.clone();
                } else {
                    idTable = task.getVotesAgain(id);//per alguna raó després de la primera iteració
                                                     //els vots dels satelits es barrejen, aleshores torno 
                                                     //a copiar-los.
                }  
                if(aux.breakThread){
                    aux.incTurn();
                    aux.notifySat(); 
                    break;
                }
                if(id == 1){
                    checkVotes();
                    if(finalVotes){
                        aux.breakThread = true;
                        aux.incTurn();
                        aux.notifySat();
                        break;
                    }                                   
                    System.out.println("ITERATION "+(aux.getIt()));
                }
                //vote and give table to the next satellite
                vote();
                nextSat.giveTable(comTable);
                //change turn                        
                if(id != nsat){                   
                    aux.incTurn();
                    aux.notifySat();
                } else {  
                    nTasksAssig = 0;
                    for(int i=0; i<ntask; i++){
                        if(comTable[i][0] != 0){
                            nTasksAssig++;
                        }
                    }                   
                    tasksAssigned.add(nTasksAssig);
                    aux.resetTurn();  
                    aux.incIt();
                    aux.notifySat();
                }
            }           
            if(id == nsat){
                File archivo = new File("output/resultadosV1_"+nsat+"_"+ntask+".txt");
                try (FileWriter w = new FileWriter(archivo,true)) {
                    Tend = System.currentTimeMillis();
                    nTasksAssig = 0;
                    for(int i=0; i<ntask; i++){
                        if(comTable[i][0] != 0){
                            nTasksAssig++;
                        }
                    }
                    System.out.println("NUMBER OF TASKS ASSIGNED: "+nTasksAssig);
                    System.out.println("NUMBER OF ITERATIONS: "+(aux.getIt()-1));
                    System.out.println("TOTAL TIME: "+(Tend-Tstart)+" ms");
                    w.write("*************************\r\n");
                    w.write("Tasks assigned\r\n");
                    w.write(Integer.toString(nTasksAssig)+"\r\n\r\n");
                    w.write("Iterations\r\n");
                    w.write(Integer.toString(aux.getIt()-1)+"\r\n\r\n");
                    w.write("Time\r\n");
                    w.write(Long.toString(Tend-Tstart)+"\r\n\r\n");
                    w.write("Tasks assigned at each iteration\r\n");
                    for(int i=0; i<aux.getIt()-1; i++){
                        w.write(Integer.toString(tasksAssigned.get(i))+"\r\n");
                    }
                    w.write("\r\n");                   
                    w.close();                    
                } 
            }           
        }catch(InterruptedException | IOException ex) {
            Logger.getLogger(Satellite.class.getName()).log(Level.SEVERE, null, ex);
        }
    }   
}
/*
            //Recibir tabla
            * Si es primera vuelta:
            1)pujar en tu propia tabla
            2)si eres el primer satelite inicializar: votoDefinitivo[0<id<nsat] = false;         
            3)pujar por tu mejor tarea en la tabla comuna
            si tu voto > otro voto, modificar tabla
            sino, pasar a votar por tu siguiente mejor tarea
            si no puedes votar por ninguna tarea, indicar que tu voto es definitivo: votoDefinitivo[id] = true;
            DUDA: quién comprueba si todos los votos son definitivos (el primer satelite o el actual?)
            si no son definitivos: pasar la tabla al siguiente
            si son definitivos: break
            4)pasar la tabla al siguiente
            
            * Si no es primera vuelta:
            1)comprobar si tu voto ha sido cambiado
            si no hay cambio: votoDefinitivo[id] = true;
            si hay cambio: votar por tu siguiente mejor tarea
            si no puedes votar, votoDefinitivo[id] = true
            DUDA: quién comprueba si todos los votos son definitivos (el primer satelite o el actual?)
            si no son definitivos: pasar la tabla al siguiente
            si son definitivos: break
            2)pasar la tabla al siguiente
            
*/