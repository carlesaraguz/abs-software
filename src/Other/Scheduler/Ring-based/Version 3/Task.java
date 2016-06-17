/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package ring.based.v3;

import java.util.ArrayList;
import java.util.Random;

/**
 *
 * @author Sebas
 * Version 3
 */
public final class Task {
    private int[] taskStorage, taskEnergy; 
    private int ntask;
    private double votes[], satVotes[][];
   
    
    public Task(int ntask, int nsat){
        this.ntask = ntask;
        taskStorage = new int[ntask];
        taskEnergy = new int[ntask];
        votes = new double[ntask]; 
        satVotes = new double[nsat][ntask];
        
    }
    public void createTaskInfo(){//(int[] storages, int[] energies)
        for(int i = 0; i<ntask; i++){
            taskStorage[i] = getStorage();
            taskEnergy[i] = getEnergy();
        }
              
    }
    public int getEnergyCapacity(){
        int enMax;
        enMax = randomInteger(9,12,new Random());
        return enMax;
    }
    
    public int getStorageCapacity(){
        int sMax;
        sMax = randomInteger(10,14,new Random());
        return sMax;
    }
    
    public int getEnergy(){
        //generate random number 1-10
        int en;
        en = randomInteger(1,10,new Random());
        return en;
    }
    
    public int getStorage(){
        //generate random number 1-11
        int s;
        s = randomInteger(1,11,new Random());
        return s;
    }
    
    public int energy(int numTask){
        return taskEnergy[numTask];
    }
    
    public int storage(int numTask){
        return taskStorage[numTask];
    }
    
    public double[] getVotesAgain(int id){
        return satVotes[id-1];
    }
    
    public double[] getVotes(double enMax, double sMax, int id){
        int i;
        for(i = 0; i<ntask; i++){
            if(taskStorage[i] > sMax || taskEnergy[i] > enMax){ 
                votes[i] = 0;
            }
            else{
                votes[i] = (taskStorage[i]/sMax + taskEnergy[i]/enMax)/2;
            } 
        }
        for(i=0;i<ntask;i++){
            satVotes[id-1][i] = votes[i];
        }      
        return votes;
    }
    
    public int randomInteger(int aStart, int aEnd, Random aRandom){
        if (aStart > aEnd) {
          throw new IllegalArgumentException("Start cannot exceed End.");
        }
        long range = (long)aEnd - (long)aStart + 1;
        long fraction = (long)(range * aRandom.nextDouble());
        int randomNumber =  (int)(fraction + aStart);          
        return randomNumber;
    } 
    
    

}
