/***************************Copyright-DO-NOT-REMOVE-THIS-LINE**
  *
  * Condor Software Copyright Notice
  * Copyright (C) 1990-2006, Condor Team, Computer Sciences Department,
  * University of Wisconsin-Madison, WI.
  *
  * This source code is covered by the Condor Public License, which can
  * be found in the accompanying LICENSE.TXT file, or online at
  * www.condorproject.org.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  * AND THE UNIVERSITY OF WISCONSIN-MADISON "AS IS" AND ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  * WARRANTIES OF MERCHANTABILITY, OF SATISFACTORY QUALITY, AND FITNESS
  * FOR A PARTICULAR PURPOSE OR USE ARE DISCLAIMED. THE COPYRIGHT
  * HOLDERS AND CONTRIBUTORS AND THE UNIVERSITY OF WISCONSIN-MADISON
  * MAKE NO MAKE NO REPRESENTATION THAT THE SOFTWARE, MODIFICATIONS,
  * ENHANCEMENTS OR DERIVATIVE WORKS THEREOF, WILL NOT INFRINGE ANY
  * PATENT, COPYRIGHT, TRADEMARK, TRADE SECRET OR OTHER PROPRIETARY
  * RIGHT.
  *
  ****************************Copyright-DO-NOT-REMOVE-THIS-LINE**/
import java.io.*;

public class x_perf_CopyFile
{
	public static int stdinwaiting;
	public static int readcount;
	public static byte[] readbuffer = new byte[4096];
	public static String source;
	public static String destination;
	public static String oneline;
	public static void main( String [] args )
	{
		// get file objects
		source = args[0];
		destination = args[1];
		File source_in = new File(source);
		File destination_out = new File(destination);

		// get stream objects
		FileInputStream in_stream = null;
		FileOutputStream out_stream = null;

		try 
		{
			in_stream = new FileInputStream(source_in);
			out_stream = new FileOutputStream(destination_out);
			while((readcount = in_stream.read(readbuffer)) != -1)
			{
				out_stream.write(readbuffer, 0, readcount);
			}
		}
		catch (IOException e) { ; }

		finally
		{
			if (in_stream != null) try { in_stream.close(); } catch (IOException e) { ; }
			if (out_stream != null) try { out_stream.close(); } catch (IOException e) { ; }
		}
	}
}

