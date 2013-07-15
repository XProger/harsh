package com.adobe.flascc {
	import flash.display.*;
	import flash.net.*;
	import flash.events.Event;
	import flash.text.TextField;
	import flash.utils.ByteArray;

	import com.adobe.flascc.vfs.ISpecialFile;
	import com.adobe.flascc.vfs.InMemoryBackingStore;
	import com.adobe.flascc.CModule;

  
	var packList:InMemoryBackingStore = new InMemoryBackingStore();
	  
	public function addPack(x:*) {
		packList.addFile("/data.jet", x);
	}
  
	/**
	* A basic implementation of a console for FlasCC apps.
	* The PlayerKernel class delegates to this for things like read/write
	* so that console output can be displayed in a TextField on the Stage.
	*/
	[SWF(frameRate = '60', width = '800', height = '600', backgroundColor = '0xffffff')]
	public class Console extends Sprite implements ISpecialFile
	{

		/**
		* To Support the preloader case you might want to have the Console
		* act as a child of some other DisplayObjectContainer.
		*/
		public function Console(container:DisplayObjectContainer = null)
		{
		  CModule.rootSprite = container ? container.root : this

		  if(CModule.runningAsWorker()) {
			return;
		  }

		  if(container) {
			container.addChild(this)
			init(null)
		  } else {
			addEventListener(Event.ADDED_TO_STAGE, init)
		  }
		}

    /**
    * All of the real FlasCC init happens in this method
    * which is either run on startup or once the SWF has
    * been added to the stage.
    */
    protected function init(e:Event):void
    {
      stage.frameRate = 60
      stage.scaleMode = StageScaleMode.NO_SCALE

      CModule.vfs.console = this;
	  CModule.vfs.addBackingStore(packList, null);
      CModule.startAsync(this)
    }

    /**
    * The callback to call when FlasCC code calls the posix exit() function. Leave null to exit silently.
    * @private
    */
    public var exitHook:Function;

    /**
    * The PlayerKernel implementation will use this function to handle
    * C process exit requests
    */
    public function exit(code:int):Boolean
    {
      // default to unhandled
      return exitHook ? exitHook(code) : false;
    }

    /**
    * The PlayerKernel implementation will use this function to handle
    * C IO write requests to the file "/dev/tty" (e.g. output from
    * printf will pass through this function). See the ISpecialFile
    * documentation for more information about the arguments and return value.
    */
    public function write(fd:int, bufPtr:int, nbyte:int, errnoPtr:int):int
    {
      var str:String = CModule.readString(bufPtr, nbyte)
      consoleWrite(str)
      return nbyte
    }

    /**
    * The PlayerKernel implementation will use this function to handle
    * C IO read requests to the file "/dev/tty" (e.g. reads from stdin
    * will expect this function to provide the data). See the ISpecialFile
    * documentation for more information about the arguments and return value.
    */
    public function read(fd:int, bufPtr:int, nbyte:int, errnoPtr:int):int
    {
      return 0
    }

    /**
    * The PlayerKernel implementation will use this function to handle
    * C fcntl requests to the file "/dev/tty" 
    * See the ISpecialFile documentation for more information about the
    * arguments and return value.
    */
    public function fcntl(fd:int, com:int, data:int, errnoPtr:int):int
    {
      return 0
    }

    /**
    * The PlayerKernel implementation will use this function to handle
    * C ioctl requests to the file "/dev/tty" 
    * See the ISpecialFile documentation for more information about the
    * arguments and return value.
    */
    public function ioctl(fd:int, com:int, data:int, errnoPtr:int):int
    {
      return 0
    }

    /**
    * Helper function that traces to the flashlog text file and also
    * displays output in the on-screen textfield console.
    */
    protected function consoleWrite(s:String):void
    {
      trace(s)
    }

    /**
    * The enterFrame callback will be run once every frame. UI thunk requests should be handled
    * here by calling CModule.serviceUIRequests() (see CModule ASdocs for more information on the UI thunking functionality).
    */
    protected function enterFrame(e:Event):void
    {
      CModule.serviceUIRequests()
    }
  }
}
