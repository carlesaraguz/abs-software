/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package ringbased;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author Sebas
 * Version 2
 */
public class Satellite extends Thread{
    private int id, ntask, nsat, enMax, sMax, nTasksAssig;
    private long Tend, Tstart;
    private double auxTable[],comTable[][];
    private double idTable[];
    private Satellite nextSat;
    private AuxClass aux;
    private Task task;
    private Double change[];
    private ArrayList<Double[]> changes;
    private ArrayList<Integer> numChanges;
    private ArrayList<Integer> tasksAssigned;
    private boolean noChanges;
    
    public Satellite(int id,int ntask, int nsat, AuxClass aux, Task task){
        this.id = id; 
        this.ntask = ntask;
        this.nsat = nsat;      
        this.aux = aux;
        this.task = task;
        idTable = new double[ntask];
        auxTable = new double[ntask];
        comTable = new double[ntask][2];      
        noChanges = false;
        change = new Double[3];
        changes = new ArrayList();
        numChanges = new ArrayList();
        tasksAssigned = new ArrayList();
    }
    
    public void getVotes(){
        idTable = task.getVotes(enMax,sMax,id);      
    }
    
    public void passChanges(ArrayList<Double[]> previousChanges){
        int i, idx;
        Double tmp[];
        i = 0;       
        if(!previousChanges.isEmpty()){
            while(i<previousChanges.size()){
                tmp = previousChanges.get(i);
                idx = (int) tmp[0].longValue();
                comTable[idx-1][0] = tmp[1];
                comTable[idx-1][1] = tmp[2];
                i++;
            }
        }
        changes = previousChanges;
    }
    
    public void vote(){
        int index = 0; 
        int tmp = 0;
        double max = 0;
        //get the first maximum vote        
        for (int k=0; k<ntask; k++){
            if(auxTable[k] > max){
                max = auxTable[k];
                index = k;                
            }
        }             
        //put the best possible vote      
        while(tmp < ntask){           
            if(comTable[index][0] < max){               
                comTable[index][0] = max;
                comTable[index][1] = id;
                change[0] = (double) index+1;
                change[1] = max;
                change[2] = (double) id;
                changes.add(change);
                break;
            }else if((comTable[index][0] == max) && (comTable[index][1] == id)){
                break;
            }            
            else{
                auxTable[index] = 0;               
                max=0;           
                for (int j=0; j<ntask; j++){
                    if(auxTable[j] > max){
                        max = auxTable[j];
                        index = j;
                    }
                }
                if(max == 0) break;
            }
            tmp++;
        }
        
    }
    
    public void setNeighbour(Satellite nextSat){
        this.nextSat = nextSat;
    }
    
    public void checkChanges(){
        if(changes.isEmpty()){
            noChanges = true;
        }
    }
    
    public void removeNextSatChanges(){
        int i;
        Double tmp[];
        i = 0;
        if(!changes.isEmpty()){
            while(i<changes.size()){
                tmp = changes.get(i);
                if(tmp[2] == 1 &&  id == nsat){
                    changes.remove(i); 
                    i--;
                }else if(tmp[2] == id+1){
                    changes.remove(i);
                    i--;
                }            
                i++;
            }
        }
    }
    
    
    @Override
    public void run(){
        Tstart = System.currentTimeMillis();    
        try{
            while(true){
                while(aux.getTurn()!=id){
                    aux.waitSat();                    
                }               
                if(aux.getIt() == 1){//get storage and energy capacity and get votes
                    enMax = task.getEnergyCapacity();
                    sMax = task.getStorageCapacity();
                    getVotes();
                    auxTable = idTable.clone(); 
                }else{
                    idTable = task.getVotesAgain(id);//per alguna raó després de la primera iteració
                                                     //els vots dels satelits es barrejen, aleshores torno 
                                                     //a copiar-los.
                }                               
                if(aux.breakThread()){//Thread breaks if breakThread = true
                    aux.incTurn();                  
                    aux.notifySat();                    
                    break;
                }                
                if(id == 1 && aux.getIt()>1){//satellite 1 checks changes minimum after the first iteration
                    checkChanges();
                    if(noChanges){                     
                        aux.setBreakThread(true);
                        aux.incTurn();                        
                        aux.notifySat();
                        break;
                    }                                    
                    System.out.println("ITERATION "+(aux.getIt()));
                }
                vote();
                removeNextSatChanges();                
                nextSat.passChanges(changes);               
                if(id != nsat){//change turn                                       
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
                    if(changes.isEmpty()){
                        numChanges.add(0);
                    } else {
                        numChanges.add(changes.size());
                    }                   
                    aux.resetTurn();  
                    aux.incIt();
                    aux.notifySat();
                }
            }           
            if(id == nsat){ 
                File archivo = new File("output/resultadosV2_"+nsat+"_"+ntask+".txt");
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
                     w.write("Number of changes at each iteration\r\n");
                    for(int i=0; i<aux.getIt()-1; i++){
                        w.write(Integer.toString(numChanges.get(i))+"\r\n");
                    }
                    w.close();                    
                }
            }            
        } catch(InterruptedException | IOException ex) {
            Logger.getLogger(Satellite.class.getName()).log(Level.SEVERE, null, ex);
        }
    }    
}
