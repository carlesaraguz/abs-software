/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package ringbased;

import java.util.Random;

/**
 *
 * @author Sebas
 * Version 2
 */
public class Task {
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
    
    public void createTaskInfo(){
        for(int i=0; i<ntask; i++){
            taskStorage[i] = getStorage();
            taskEnergy[i] = getEnergy();
        }
    }
    
    public int getEnergyCapacity(){
        int enMax;
        enMax = randomInteger(10,14,new Random());
        return enMax;
    }
    
    public int getStorageCapacity(){
        int sMax;
        sMax = randomInteger(8,12,new Random());
        return sMax;
    }
    
    public int getEnergy(){
        //generate random number from 1-12
        int en;
        en = randomInteger(1,12,new Random());
        return en;
    }
    
    public int getStorage(){
        //generate random number from 1-10
        int s;
        s = randomInteger(1,10,new Random());
        return s;
    }
    
    public double[] getVotesAgain(int id){
        return satVotes[id-1];
    }
    
    public double[] getVotes(double enMax, double sMax, int id){
        int i;
        for(i=0; i<ntask; i++){
            if(taskStorage[i] > sMax || taskEnergy[i] > enMax){ 
                votes[i] = 0.0;
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
