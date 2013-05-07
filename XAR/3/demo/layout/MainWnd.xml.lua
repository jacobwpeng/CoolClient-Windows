

function OnMainWndSize(self,type_, width, height)
	if Type == "min" then
		---最小化时w h 为0 不要用这个重设obj pos 否则各种异常
		return
	elseif Type == "max" then
		local tree = self:GetBindUIObjectTree()
		if tree ~= nil then
			local sysButton = tree:GetUIObject( "SystemBtn" )
			if sysButton ~= nil then
				sysButton:SetMaxState( false )
			end
		end
	elseif Type == "restored" then
		local tree = self:GetBindUIObjectTree()
		if tree ~= nil then
			local sysButton = tree:GetUIObject( "SystemBtn" )
			if sysButton ~= nil then
				sysButton:SetMaxState( true )
			end
		end
	end
	
	local objectTree = self:GetBindUIObjectTree()
	local rootObject = objectTree:GetRootObject()
	local left,top,right,bottom = objectTree:GetUIObject("ButtonPage"):GetObjPos()
	rootObject:SetObjPos(0, 0, width, height)
	XLMessageBox(right-left)
end

function OnClickClose(self)
	os.exit(0)
end

function OnClickTextLink(self)
	XLMessageBox("但我们不负责维护哦亲!")
end

function OnProgressInitControl(self)
	
	timer = XLGetObject("Xunlei.UIEngine.TimerManager")
	timer:SetTimer(function (x)
		local newProgress = self:GetProgress()
		if newProgress >= 100 then
			newProgress = 0
		else
			newProgress = newProgress + 1
		end
		self:SetProgress(newProgress)
	end,100,true)
end

function OnMinisize(self)
	local hostwnd = self:GetOwner():GetBindHostWnd()
	hostwnd:Min() 
end

function OnMaxSize(self)
	local hostWnd = self:GetOwner():GetBindHostWnd()
	hostWnd:Max()
	self:SetMaxState( false )
end

function OnReStore(self)
	local hostWnd = self:GetOwner():GetBindHostWnd()
	hostWnd:Restore()
	self:SetMaxState( true )
end

function OnClose(self)
	os.exit(0)
end

function OnInitTabHeader(self)
	local tabbkg = self:GetOwner():GetUIObject("tabbkg")
	
	self:AddTabItem("ButtonPage","按钮")
	self:AddTabItem("ProgressPage","进度条","bitmap.pin")
	self:AddTabItem("ButtonPage1","按钮")
	
	tabbkg:AddPage("Demo.ButtonPage","ButtonPage")
	tabbkg:AddPage("Demo.ProgressPage","ProgressPage")
	tabbkg:AddPage("Demo.ButtonPage","ButtonPage1")
	

	AsynCall(function (x) tabbkg:ActivePage("ButtonPage") end)

end

function OnActiveTabChanged(self,eventName,newID,oldID)
	local tabbkg = self:GetOwner():GetUIObject("tabbkg")
	tabbkg:ActivePage(newID)
end