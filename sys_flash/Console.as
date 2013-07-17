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

		public var exitHook:Function;

		public function exit(code:int):Boolean {
			return exitHook ? exitHook(code) : false;
		}

		public function write(fd:int, bufPtr:int, nbyte:int, errnoPtr:int):int {
			var str:String = CModule.readString(bufPtr, nbyte);
			consoleWrite(str);
			return nbyte;
		}

		public function read(fd:int, bufPtr:int, nbyte:int, errnoPtr:int):int {
			return 0;
		}

		public function fcntl(fd:int, com:int, data:int, errnoPtr:int):int {
			return 0;
		}

		public function ioctl(fd:int, com:int, data:int, errnoPtr:int):int {
			return 0;
		}

		protected function consoleWrite(s:String):void {
			trace(s);
		}

		protected function enterFrame(e:Event):void {
			CModule.serviceUIRequests();
		}
	}
}