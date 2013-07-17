package com.adobe.flascc {
	import flash.display.*;
	import flash.net.*;
	import flash.events.Event;
	import flash.events.KeyboardEvent;
	import flash.events.MouseEvent;
	import flash.text.TextField;
	import flash.utils.ByteArray;

	import com.adobe.flascc.vfs.ISpecialFile;
	import com.adobe.flascc.vfs.InMemoryBackingStore;
	import com.adobe.flascc.CModule;

  
	var packList:InMemoryBackingStore = new InMemoryBackingStore();
	  
	public function addPack(x:*) {
		packList.addFile("/data.jet", x);
	}

	public class Console extends Sprite implements ISpecialFile  {
		var touchProc:int;
	
		public function Console(container:DisplayObjectContainer = null)
		{
			CModule.rootSprite = container ? container.root : this

			if(CModule.runningAsWorker())
				return;

			if (container) {
				container.addChild(this);
				init(null);
			} else
				addEventListener(Event.ADDED_TO_STAGE, init);
		}


		protected function init(e:Event):void {
			touchProc = CModule.getPublicSymbol("touchProc");

			CModule.vfs.console = this;
			CModule.vfs.addBackingStore(packList, null);
			CModule.startAsync(this)
			
			stage.addEventListener(KeyboardEvent.KEY_DOWN, onKeyDown);
			stage.addEventListener(KeyboardEvent.KEY_UP, onKeyUp);
			stage.addEventListener(MouseEvent.MOUSE_MOVE, onMouseMove);
			stage.addEventListener(MouseEvent.MOUSE_DOWN, onMouseDown);
			stage.addEventListener(MouseEvent.MOUSE_UP, onMouseUp);
		}

		public function touch(id:int, state:int, x:int, y:int) {
			CModule.callI(touchProc, new <int>[id, state, x, y]);
		}

		public function onMouseMove(me:MouseEvent):void {
			me.stopPropagation();
			touch(0, 2, me.stageX, me.stageY);
		}
    
		public function onMouseDown(me:MouseEvent):void  {
			me.stopPropagation();
			touch(0, 0, me.stageX, me.stageY);
		}
    
		public function onMouseUp(me:MouseEvent):void {
			me.stopPropagation();
			touch(0, 1, me.stageX, me.stageY);
		}

		public function onKeyDown(ke:KeyboardEvent):void  {
			touch(ke.keyCode, 3, 0, 0);
		}

		public function onKeyUp(ke:KeyboardEvent) {
			touch(ke.keyCode, 4, 0, 0);
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
