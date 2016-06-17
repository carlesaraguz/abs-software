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
public class ringBasedV3 {
    /**
     * @param args the command line arguments
     * @throws java.lang.InterruptedException
     */
    public static void main(String[] args) throws InterruptedException {          
        int ntask = Integer.parseInt(args[1]);
        int nsat = Integer.parseInt(args[0]);        
        Task task = new Task(ntask,nsat);
        task.createTaskInfo();//pasar datos de args (taskStorage y taskEnergy)
        AuxClass aux = new AuxClass(nsat);
        Thread[] t = new Thread[nsat];
        Satellite[] sat = new Satellite[nsat];       
        //create satellites
        for(int j=0; j<nsat; j++){
            sat[j] = new Satellite(j+1, ntask, nsat, aux, task);//j+1 = id                  
        } 
        //set neighbours
        for(int k=0; k<nsat; k++){
            if(k == nsat-1){
                sat[k].setNeighbour(sat[0]);
            }else{
                sat[k].setNeighbour(sat[k+1]);
            }    
        }    
        //initialize threads
        for (int i=0; i<nsat; i++){  
            t[i] = new Thread(sat[i]);
            t[i].start(); 
            if(i == nsat-1){
                t[i].join();
            }
        }  
    }
    
}
