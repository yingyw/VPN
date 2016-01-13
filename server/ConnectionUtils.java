import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.stream.Stream;


public class ConnectionUtils {
	public static byte[] verifyAndGetData(byte[] data, int messageLength, int magicNumber) {
		if (data.length != messageLength) {
			// the size is not the same with the protocol
			throw new IllegalStateException("Expect " + messageLength + " "
					+ "bytes, actual is " + data.length );
		}

		ByteBuffer buffer = ByteBuffer.wrap(data).order(ByteOrder.LITTLE_ENDIAN);

		int magicNumberHandshake1 = buffer.getInt();
		if (magicNumberHandshake1 != magicNumber) {
			throw new IllegalStateException(" the magic number is not the same, message might be corrupted, expected " +
					magicNumber + ", but got " + magicNumberHandshake1);
		}

//		System.out.println(" data length " + data.length);
//		System.out.println(" message Length " + messageLength);
		
		byte[] message = new byte[messageLength - 4];
		buffer.get(message);
		
		//System.out.println(" declared message Length " + message.length);


		return message;
	}
	
	public static byte[] readFromClient(InputStream clientInputStream) {
		try {
			byte[] lengthBytes = new byte[4];
			
			clientInputStream.read(lengthBytes, 0, 4);
			int length = ByteBuffer.wrap(lengthBytes).order(ByteOrder.LITTLE_ENDIAN).getInt(0);
			
//			for (int i = 0; i < 4; i++)
//				System.out.println(String.format("%02X ", lengthBytes[i]));
			
			//System.out.println("length get from the client " + length);
			
			length -= 4;
			
			byte[] bytes = new byte[length];
			clientInputStream.read(bytes);
			return bytes;
		} catch (IOException e) {
			e.printStackTrace();
			return new byte[0];
		}
	}
}
