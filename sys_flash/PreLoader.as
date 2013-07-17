package com.adobe.flascc.preloader {
	import flash.display.*;
	import flash.events.*;
	import flash.events.*;
	import flash.net.*;
	import flash.system.ApplicationDomain;
	import flash.text.*;
	import flash.utils.ByteArray;
	import flash.media.Sound;
	
	import com.adobe.flascc.Console;
    
	[SWF(backgroundColor="#000000")]
	public class PreLoader extends MovieClip {
		var fail = false
		private var datazips:Array = [];
		private var engineLoaded:Boolean = false;

		var vfscomplete:Boolean, enginecomplete:Boolean

		public function PreLoader() {
			addEventListener(Event.ADDED_TO_STAGE, onAddedToStage);
		}

		private function onAddedToStage(e:*) {
			try {
				stage.addEventListener(MouseEvent.RIGHT_CLICK, function (e:*):void {} );
			} catch(e:*) {}
			
			var s:Sound = new Sound( new URLRequest("music.mp3") );
			s.play(0, 999);
			
			stage.frameRate = 60
			loaderInfo.addEventListener(ProgressEvent.PROGRESS, onProgress4)

			var datazip1 = new URLLoader(new URLRequest("data.jet"));
			datazip1.dataFormat = URLLoaderDataFormat.BINARY;
			datazip1.addEventListener(AsyncErrorEvent.ASYNC_ERROR, onError)
			datazip1.addEventListener(SecurityErrorEvent.SECURITY_ERROR, onError)
			datazip1.addEventListener(IOErrorEvent.IO_ERROR, onError)
			datazip1.addEventListener(Event.COMPLETE, onComplete)
			datazip1.addEventListener(ProgressEvent.PROGRESS, onProgress1)
		}

		private var p1:int, p4:int, ps1:int = 1, ps4:int = 1;

		private function onProgress1(e:ProgressEvent):void {
			ps1 = e.bytesTotal;
			p1 = e.bytesLoaded;
			render()
		}

		private function onProgress4(e:ProgressEvent):void {
			ps4 = e.bytesTotal;
			p4 = e.bytesLoaded;
			render()
		}

		private function onComplete(e:Event):void {
			datazips.push(e.target.data);
			if (datazips.length == 1)
				addEventListener(Event.ENTER_FRAME, enterFrame);
			render()
		}

		private function render():void {
			graphics.clear();
			graphics.beginFill(0);
			graphics.drawRect(0, 0, stage.stageWidth, stage.stageHeight);
			graphics.endFill();

			var pct:Number = (p1 + p4) / (ps1 + ps4);
			var barColor = fail ? 0xFF0000 : 0xFFFFFF;

			var barHeight:int = 40;
			var barWidth:int = stage.stageWidth * 0.75;

			graphics.lineStyle(1, barColor);
			graphics.drawRect((stage.stageWidth - barWidth) / 2, (stage.stageHeight / 2) - (barHeight / 2), barWidth, barHeight);

			graphics.beginFill(barColor);
			graphics.drawRect((stage.stageWidth - barWidth) / 2 + 5, (stage.stageHeight / 2) - (barHeight / 2) + 5, (barWidth - 10) * pct, barHeight - 10);
			graphics.endFill();
		}

		private function onError(e:Event):void {
			fail = true
			render();
		}

		private function enterFrame(e:Event):void {
			if (fail || datazips.length < 1)
				return;

			removeEventListener(Event.ENTER_FRAME, enterFrame);
			gotoAndStop(2);
			graphics.clear();

			var f = ApplicationDomain.currentDomain.getDefinition("com.adobe.flascc.addPack");
			for (var i in datazips)
				f(datazips[i]);
			datazips = null;

			new Console(this);
		}
	}
}
