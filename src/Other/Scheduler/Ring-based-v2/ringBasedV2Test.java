/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package ringbased;


import static java.lang.Thread.sleep;
import junit.framework.TestCase;

/**
 *
 * @author Sebas
 */
public class ringBasedV2Test extends TestCase {
    
    public ringBasedV2Test(String testName) {
        super(testName);
    }
    
    @Override
    protected void setUp() throws Exception {
        super.setUp();
    }
    
    @Override
    protected void tearDown() throws Exception {
        super.tearDown();
    }

    /**
     * Test of main method, of class ringBasedV2.
     * @throws java.lang.InterruptedException
     */
    public void testMain() throws InterruptedException{       
        int nsatmin = 100;
        int nsatmax = 100;
        int nsatStep = 20;
        
        int ntaskmin = 0;
        int ntaskmax = 100;      
        int ntaskStep = 20;
        
        int numrep = 1;
        
        String[] args = new String[2];        
        for(int i = nsatmin; i<= nsatmax; i = i+nsatStep){
            for(int j = ntaskmin; j<= ntaskmax; j = j+ntaskStep){
                for(int k=1; k<=numrep; k++){
                    if(i==0){
                        args[0] = Integer.toString(1);
                    } else {
                        args[0] = Integer.toString(i);
                    }
                    if(j==0){
                        args[1] = Integer.toString(1);
                    } else {
                        args[1] = Integer.toString(j);
                    }                   
                    ringBasedV2.main(args);  
                    sleep(2000);
                }
            }
        } 
        // TODO review the generated test code and remove the default call to fail.
        //fail("The test case is a prototype.");
    }
    
}
