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
		
		/*BufferedImage imageOut = doMedianFilter(imageIn, maskSize, fileOut);
			
		try {
			ImageIO.write(imageOut, "BMP", fileOut);
		} catch (IOException e) {
			e.printStackTrace();
		}*/
	}
	
	public static QuickSort quickSort = new QuickSort(); 
	
	/*
	private static BufferedImage doMedianFilter(BufferedImage image, int maskSize, File file) {

		BufferedImage imageOut = image;
		
		int color_blue[] = new int[maskSize * maskSize],
			color_green[] = new int[maskSize * maskSize],
			color_red[] = new int[maskSize * maskSize];
				
	    for (int row = 0; row < image.getHeight(); row++) {
	      for (int col = 0; col < image.getWidth(); col++) {

	         if (row < (maskSize/2) || row >= image.getHeight() - (maskSize/2)
	            || col < (maskSize/2) || col >= image.getWidth() - (maskSize/2)) {
	        	 Color c = new Color(image.getRGB(col, row));
	        	 imageOut.setRGB(col, row, (c.getRed() << 16 | c.getGreen() << 8 | c.getBlue()));	        	 
	             continue;
	         }

	         int point = 0;
	         for (int rowAux =- (maskSize/2); rowAux <= (maskSize/2); rowAux++) {
	            for (int colAux =- (maskSize/2); colAux <= (maskSize/2); colAux++) {
	            	Color c = new Color(image.getRGB(col + colAux, row + rowAux));	               
	            	color_red[point] = c.getRed();
	            	color_green[point] = c.getGreen();
	                color_blue[point] = c.getBlue();
	                point++;
	            }
	         }

	         quickSort.sort(color_red, 0, point - 1);
	         quickSort.sort(color_green, 0, point - 1);
	         quickSort.sort(color_blue, 0, point - 1);
	         imageOut.setRGB(col, row, (color_red[point/2] << 16 | color_green[point/2] << 8 | color_blue[point/2]));
	      }
	   }	    
		
		return imageOut;
	}
		*/
}