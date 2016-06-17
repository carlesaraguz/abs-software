/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ring.based.v3;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author Sebas 
  Version 3
 */
public class Satellite extends Thread {
    private int id, ntask, nsat, enMax, sMax, nTasksAssig, sMaxTotal, eMaxTotal;
    private long Tend, Tstart;
    private double idTable[], auxTable[],comTable[][], orderedVotes[][];
    private Satellite nextSat;
    private AuxClass aux;
    private Task task;
    private Double change[],auxChange[];
    private ArrayList<Double[]> changes;   
    private ArrayList<Integer> tasksAssigned,numChanges,totalFreeStorage, totalFreeEnergy;  
    private boolean noChanges, tasksVoted[], taskAlreadyVoted[];

    public Satellite(int id, int ntask, int nsat, AuxClass aux, Task task) {
        this.id = id;
        this.ntask = ntask;
        this.nsat = nsat;
        this.aux = aux;
        this.task = task;
        idTable = new double[ntask];
        auxTable = new double[ntask];
        comTable = new double[ntask][2];
        orderedVotes = new double[ntask][2];
        tasksVoted = new boolean[ntask];
        noChanges = false;
        change = new Double[3];
        auxChange = new Double[3];
        changes = new ArrayList();   
        tasksAssigned = new ArrayList();
        numChanges = new ArrayList();
        totalFreeStorage = new ArrayList();
        totalFreeEnergy = new ArrayList();
        taskAlreadyVoted = new boolean[ntask];
    }

    public void getVotes() {
        idTable = task.getVotes(enMax,sMax,id);
    }

    public void passChanges(ArrayList<Double[]> previousChanges) {      
        int i, idx;
        Double tmp[];
        i = 0;
        if (!previousChanges.isEmpty()) {
            while (i < previousChanges.size()) {
                tmp = previousChanges.get(i);
                idx = (int) tmp[0].longValue();
                comTable[idx - 1][0] = tmp[1];
                comTable[idx - 1][1] = tmp[2];
                i++;
            }
        }
        changes = previousChanges;
    }
    
    public void orderVotes(){
        int k = 0;
        double max = 0;
        int index = 0;
        while(k < ntask){
            for (int i = 0; i < ntask; i++) {
                if (auxTable[i] > max) {
                    max = auxTable[i];
                    index = i;
                }
            }           
            orderedVotes[k][0] = max;
            orderedVotes[k][1] = index+1;//index + 1 = task number
            auxTable[index] = 0;
            max = 0;
            k++;
        }       
    }
        
    public void vote() {
        int index;
        int j,k;     
        if(aux.getIt() == 1){ 
            orderVotes();
        }
        /*put the best possible votes*/
        k = 0;
        while (k < ntask) {            
            if(k == 0 && aux.getIt() > 1) {//restore capacity if other satellite assigned 
                j = 0;                     //itself a task that was voted previously by this satellite 
                while(j < ntask){
                    index = (int) orderedVotes[j][1] - 1;
                    if(tasksVoted[index] && (comTable[index][0] > orderedVotes[j][0])){                                             
                        sMax = sMax + task.storage(index);
                        enMax = enMax + task.energy(index);
                        tasksVoted[index] = false;
                    }
                    j++;
                }
            }
            index = (int) orderedVotes[k][1] - 1;           
            if(orderedVotes[k][0] == 0){
                break;
            }
            else if((comTable[index][0] < orderedVotes[k][0]) && (task.storage(index) <= sMax && task.energy(index) <= enMax)){              
                comTable[index][0] = orderedVotes[k][0];
                comTable[index][1] = id;
                auxChange[0] = (double) (index + 1);
                auxChange[1] = orderedVotes[k][0];
                auxChange[2] = (double) id;
                change = auxChange.clone();
                changes.add(change);
                tasksVoted[index] = true;
                sMax = sMax - task.storage(index);
                enMax = enMax - task.energy(index);
            }                                
            k++;
        }  
    }

    public void setNeighbour(Satellite nextSat) {
        this.nextSat = nextSat;
    }

    public void checkChanges() {
        if (changes.isEmpty()){ 
            noChanges = true;  
        }
    }

    public void removeNextSatChanges() {
        int i = 0;
        Double tmp[];
        if (!changes.isEmpty()) {
            while (i < changes.size()) {
                tmp = changes.get(i);
                if (tmp[2] == 1 && id == nsat) {
                    changes.remove(i);
                    i--;
                } else if (tmp[2] == id + 1) {
                    changes.remove(i);
                    i--;
                } 
                i++;
            }
        }
    }

    @Override
    public void run() {
        Tstart = System.currentTimeMillis();
        try {
            while (true) {
                while (aux.getTurn() != id) {
                    aux.waitSat();
                }
                if (aux.breakThread()) { //Thread breaks if breakThread = true
                    aux.incTurn();
                    aux.notifySat();
                    break;
                }                
                if (aux.getIt() == 1) {//get storage and energy capacity and get votes
                    enMax = task.getEnergyCapacity();
                    sMax = task.getStorageCapacity();
                    aux.passCapacity(id, sMax, enMax);
                    getVotes();
                    auxTable = idTable.clone();
                } else {
                    idTable = task.getVotesAgain(id);//per alguna raó després de la primera iteració
                                                     //els vots dels satelits es barrejen, aleshores torno 
                                                     //a copiar-los.
                }               
                if (id == 1 && aux.getIt()>1) {//satellite 1 checks changes
                    checkChanges();
                    if (noChanges) {
                        aux.setBreakThread(true);
                        aux.incTurn();
                        aux.notifySat();
                        break;
                    }
                }            
                vote();//vote and pass changes to the next satellite
                removeNextSatChanges();              
                nextSat.passChanges(changes);                                       
                if (id != nsat){//change turn                                      
                    aux.incTurn();
                    aux.notifySat();
                } else {
                    if(aux.getIt() == 1){
                        eMaxTotal = aux.getMaxCapacities()[0];
                        sMaxTotal = aux.getMaxCapacities()[1];
                        totalFreeEnergy.add(eMaxTotal);
                        totalFreeStorage.add(sMaxTotal);
                    }                      
                    nTasksAssig = 0;
                    for(int i=0; i<ntask; i++){
                        if(comTable[i][0] != 0){
                            if(!taskAlreadyVoted[i]){
                                taskAlreadyVoted[i] = true;
                                eMaxTotal = eMaxTotal - task.energy(i);
                                sMaxTotal = sMaxTotal - task.storage(i); 
                            }
                            nTasksAssig++;                         
                        } 
                    }                   
                    tasksAssigned.add(nTasksAssig);                  
                    if(changes.isEmpty()){
                        numChanges.add(0);
                    } else {
                        numChanges.add(changes.size());
                    } 
                    totalFreeEnergy.add(eMaxTotal);
                    totalFreeStorage.add(sMaxTotal);
                    aux.resetTurn(); 
                    aux.incIt();
                    aux.notifySat();
                }
            }
            if (id == nsat) {
                File archivo = new File("output/resultadosV3_"+nsat+"_"+ntask+".txt");
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
                    w.write("\r\nNumber of changes at each iteration\r\n");
                    for(int i=0; i<aux.getIt()-1; i++){
                        w.write(Integer.toString(numChanges.get(i))+"\r\n");
                    }                   
                    w.write("\r\nFree energy capacity at each iteration\r\n");
                    for(int i=0; i<aux.getIt(); i++){
                        w.write(totalFreeEnergy.get(i)+"\r\n");
                    }
                    w.write("\r\nFree energy capacity at each iteration\r\n");
                    for(int i=0; i<aux.getIt(); i++){
                        w.write(totalFreeStorage.get(i)+"\r\n");
                    }
                    w.close();
                } 
            }
        } catch(InterruptedException | IOException ex) {
            Logger.getLogger(Satellite.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}
