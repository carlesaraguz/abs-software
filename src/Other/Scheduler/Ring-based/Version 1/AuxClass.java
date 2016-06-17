/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package ring.based.v1;

/**
 *
 * @author SebAs
 * Version 1
 */
public class AuxClass{
    private int it,turn,nsat;
    private boolean[] changed;
    public boolean breakThread;
    
    public AuxClass(int nsat){
        this.nsat = nsat;
        it = 1;
        turn = 1;
        changed = new boolean[nsat];
        for(int i=0; i<nsat; i++){
            changed[i] = true;
        }
        breakThread = false;
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
    
    public void setChangeTo(boolean c, int id){
        changed[id-1] = c;
    }
    
    public boolean votesChanged(){
        int i=0;      
        while(!changed[i]){
            i++;
            if(i == nsat) break;
        }
        if(i == nsat){
            return changed[nsat-1];
        } else {
            return changed[i];
        }
    }
    
    public synchronized void waitSat() throws InterruptedException{
        wait();
    }
    
    public synchronized void notifySat(){
        notifyAll();
    }
}
