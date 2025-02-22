import java.awt.Color;
import java.awt.image.BufferedImage;
import java.io.File;

public class MedianFilter extends Thread {
	private int tid;
	private int nthreads;
	private BufferedImage image; 
	private int maskSize;
	private File file;
	private BufferedImage imageOut;
	
	public static QuickSort quickSort = new QuickSort(); 

	public MedianFilter(int tid, int nthreads, BufferedImage image, int maskSize, File file){
		this.tid = tid;
		this.nthreads = nthreads;
		this.image = image; 
		this.maskSize = maskSize;
		this.file = file;
		this.imageOut = null;
	}
	
	public BufferedImage getMedianFilter(){
		return (this.imageOut);
	}
	
	public void run() {
		
		BufferedImage imageOut = image;
		
		int color_blue[] = new int[maskSize * maskSize],
			color_green[] = new int[maskSize * maskSize],
			color_red[] = new int[maskSize * maskSize];
				
	    for (int row = tid; row < image.getHeight(); row += nthreads) {
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
	    
	    this.imageOut = imageOut;
		
	}
}
