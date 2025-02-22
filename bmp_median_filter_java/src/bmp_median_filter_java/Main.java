import java.awt.Color;
import java.awt.image.BufferedImage;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;

import javax.imageio.ImageIO;

public class Main {
	
    public static void main(String [] args){
				if ( args.length == 0 ){
			System.out.printf("Passar o numero de termos e threads como argumento\n");
			return;
		}
		
		
		
		if (args.length != 4) {
			System.out.printf("%s <imagem_de_entrada> <imagem_de_saida> <tamanho_mascara> <numero_threads>", args[0]);
			System.exit(0);
		}
		
		int maskSize = Integer.parseInt(args[2]); 
		int nthreads = Integer.parseInt(args[3]);
		
		File fileIn = new File(args[0]);
		File fileOut = new File(args[1]);
		BufferedImage imageIn = null;
		try {
			imageIn = ImageIO.read(fileIn);			
		} 
		catch (IOException e) {
			e.printStackTrace();			
		}
		
		MedianFilter [] p = new MedianFilter[nthreads];

		for(int i=0; i<nthreads; i++){
			p[i] = new MedianFilter(i,nthreads,imageIn, maskSize, fileOut);
			p[i].start();
			
		}	

		for(int i=0; i<nthreads; i++){
			try{				
				p[i].join();
			}
			catch(InterruptedException e){
				e.printStackTrace();
			}
			
			try {
				ImageIO.write(p[i].getMedianFilter(), "BMP", fileOut);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		

	}
	
}
