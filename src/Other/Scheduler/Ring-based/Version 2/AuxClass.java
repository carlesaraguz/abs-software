/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package ringbased;

/**
 *
 * @author Sebas
 * Version 2
 */
public class AuxClass {
    private int it,turn;
    private boolean[] changed;
    private boolean breakThread;
    
    public AuxClass(int nsat){
        it = 1;
        turn = 1;
        changed = new boolean[nsat];
        for(int i=0; i<nsat; i++)
            changed[i] = true;
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
}
