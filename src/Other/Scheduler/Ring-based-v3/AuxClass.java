/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package ring.based.v3;

/**
 *
 * @author Sebas
 * Version 3
 */
public class AuxClass {
    private int it, turn,eMaxTotal, sMaxTotal;
    private boolean[] changed;
    private boolean breakThread;
    private int[] freeStorages, freeEnergies;
    
    public AuxClass(int nsat){   
        eMaxTotal = 0;
        sMaxTotal = 0;
        it = 1;
        turn = 1;
        changed = new boolean[nsat];
        for(int i=0; i<nsat; i++){
            changed[i] = true;
        }
        breakThread = false;
        freeStorages = new int[nsat];
        freeEnergies = new int[nsat];     
    }
    
    public synchronized void incIt(){
        it++;
    }
    
    public void resetIt(){
        it = 1;
    }
    
    public int getIt(){
        return it;
    }
    
    public synchronized void incTurn(){
        turn++;
    }
    
    public int getTurn(){
        return turn;
    }
    
    public void resetTurn(){
        turn = 1;
    }
    
    public synchronized void waitSat() throws InterruptedException{
        wait();
    }
    
    public synchronized void notifySat(){
        notifyAll();
    }  
    
    public void setBreakThread(boolean b){
        breakThread = b;
    }
    
    public boolean breakThread(){
        return breakThread;
    } 
    
    public void passCapacity(int id,int sMax, int enMax){ 
        eMaxTotal = eMaxTotal + enMax;
        sMaxTotal = sMaxTotal + sMax;
        freeStorages[id-1] = sMax;
        freeEnergies[id-1] = enMax;
    }
    
    public int[] getMaxCapacities(){
        int[] tmp = new int[2];
        tmp[0] = eMaxTotal;
        tmp[1] = sMaxTotal;
        return tmp;
    }
}
