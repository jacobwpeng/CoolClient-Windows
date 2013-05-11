function OnTipsInitControl(self)
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local tipsHostWndTemplate = templateMananger:GetTemplate("Thunder.Tips","HostWndTemplate")
	tipsHostWndTemplate:CreateInstance("Thunder.Tips.Instance")
end

--窗口大小改变
function OnMainWndSize(self,type_, width, height)
	local objectTree = self:GetBindUIObjectTree()
	local rootObject = objectTree:GetRootObject()
	
	rootObject:SetObjPos(0, 0, width, height)
	
	local tabbkg = objectTree:GetUIObject("tabbkg")
	local left,top,right,bottom = tabbkg:GetObjPos()
	local bkgwidth = right-left
	--AsynCall(function() XLMessageBox("width:"..width.." bkgwidth:"..bkgwidth.."PageWidth:") end)
end
--初始化主窗口的属性
function OnMainWndCreate(self)
	local tab = {TabNum = 1, ActiveTab = "mainwnd.tab.mydownload", MydownloadPage = false,
		SearchPage = false, ConfigPage = false, Cron, Config={}}
	self:SetUserData(tab)
	--XLMessageBox(tab.panelState)
end
--tips demo
function OnMouseHover(self, x, y)	
	local hostWndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local tipsHostWnd = hostWndManager:GetHostWnd("Thunder.Tips.Instance")
						
	tipsHostWnd:DelayPopup(600)
	tipsHostWnd:SetPositionByObject(50,30,self)
	
    local objTree = self:GetOwner()
    local image = objTree:GetUIObject("newtask.button")
	local left, top, right, bottom = image:GetObjPos()
    if image then
        local aniFactory = XLGetObject("Xunlei.UIEngine.AnimationFactory")
        if aniFactory then
            local seqAnim = aniFactory:CreateAnimation("SeqFrameAnimation")
            seqAnim:BindImageObj(image)
            seqAnim:SetResID("imageseq.newtask.ani")
            seqAnim:SetTotalTime(500)
            local cookie = seqAnim:AttachListener(true,function (self,oldState,newState)
                end)
            objTree:AddAnimation(seqAnim)
            seqAnim:Resume()
        end
    end	

end

function OnMouseLeave(self, x, y)
    local objTree = self:GetOwner()
    local image = objTree:GetUIObject("newtask.button")
	--local left, top, right, bottom = image:GetObjPos()
    if image then
        local aniFactory = XLGetObject("Xunlei.UIEngine.AnimationFactory")
        if aniFactory then
            local seqAnim = aniFactory:CreateAnimation("SeqFrameAnimation")
            seqAnim:BindImageObj(image)
            seqAnim:SetResID("imageseq.newtask.ani")
            seqAnim:SetTotalTime(500)
			seqAnim:SetReverse(true)
            local cookie = seqAnim:AttachListener(true,function (self,oldState,newState)
                end)
            objTree:AddAnimation(seqAnim)
            seqAnim:Resume()
        end
    end	
	local hostWndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local tipsHostWnd = hostWndManager:GetHostWnd("Thunder.Tips.Instance")
	tipsHostWnd:DelayCancel(0)
end

--用来实现关闭按钮的相关事件，包括贴图和关闭程序
function OnSystemCloseButtonLButtonUp(self, x, y, flags)
	local bkg = self:GetControlObject("button.bkg")
	local attr = self:GetAttribute()
	bkg:SetResID(attr.HoverBkg)
	if x>=0 and x<=41 and y<=19 and y>=0 then
		local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
		coolClientProxy:StopClient()
		os.exit()
	end
end

--用来实现最大化按钮的相关事件
function OnSystemMaximumButtonLButtonUp(self, x, y, flags)
	local bkg = self:GetControlObject("button.bkg")
	local attr = self:GetAttribute()	
	bkg:SetResID(attr.NormalBkg)
end

--用来实现最小化按钮的相关事件
function OnSystemMinimumButtonLButtonUp(self, x, y, flags)
	local bkg = self:GetControlObject("button.bkg")
	local attr = self:GetAttribute()
	bkg:SetResID(attr.NormalBkg)
	if x>=0 and x<=27 and y<=19 and y>=0 then	
		local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
		local hostwnd = hostwndManager:GetHostWnd("MainFrame")
		hostwnd:Min() 
	end
end

--用来实例化系统菜单
function OnSystemConfigButtonClick(self)
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")				
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local parent = hostwndManager:GetHostWnd("MainFrame")
	local parentHandle = parent:GetWndHandle()
	
	local menuTreeTemplate = templateMananger:GetTemplate("Thunder.MainMenu","ObjectTreeTemplate")
	local menuTree = menuTreeTemplate:CreateInstance("Thunder.MainMenu.Instance")
	local menuHostWndTemplate = templateMananger:GetTemplate("Thunder.MainMenuHostwnd", "HostWndTemplate")
	local menuHostWnd = menuHostWndTemplate:CreateInstance("Thunder.MenuHostWnd.Instance")
	
	menuHostWnd:BindUIObjectTree(menuTree)
	
	--获取父窗口的宽度还未实现，更无法实现窗口可变
	local x, y = parent:ClientPtToScreenPt(740, 25)
	menuHostWnd:TrackPopupMenu(parentHandle, x, y, x+400, y+400)
	
	local objtreeManager = XLGetObject("Xunlei.UIEngine.TreeManager")	
	objtreeManager:DestroyTree("Thunder.MainMenu.Instance")
	hostwndManager:RemoveHostWnd("Thunder.MenuHostWnd.Instance")
end

--新建任务模态对话框的弹出
function OnNewTaskButtonClick(self)
	local templateManager = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local hostWndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local mainWnd = hostWndManager:GetHostWnd("MainFrame")
	local modalHostWndTemplate = templateManager:GetTemplate("Thunder.ModalDlg","HostWndTemplate")
	local modalHostWnd = modalHostWndTemplate:CreateInstance("Thunder.ModalDlg.Instance")
	local objectTreeTemplate = templateManager:GetTemplate("Thunder.ModalDlg","ObjectTreeTemplate")
	local uiObjectTree = objectTreeTemplate:CreateInstance("Thunder.ModalDlg.Instance")
	modalHostWnd:BindUIObjectTree(uiObjectTree)
	
	
	--local userdata = mainWnd:GetUserData()
	modalHostWnd:DoModal(mainWnd:GetWndHandle())
	--XLMessageBox(mainWnd:GetWndHandle().top)
	
	local objtreeManager = XLGetObject("Xunlei.UIEngine.TreeManager")	
	objtreeManager:DestroyTree("Thunder.ModalDlg.Instance")
	hostWndManager:RemoveHostWnd("Thunder.ModalDlg.Instance")
end

function OnInitTabHeader(self)
	local tabbkg = self:GetOwner():GetUIObject("tabbkg")
	
	self:AddTabItem("MydownloadPage","我的下载","bitmap.tab.mydownload")
	tabbkg:AddPage("MydownloadPage","MydownloadPage")
	--local left,top,right,bottom = self:GetObjPos()
	--XLMessageBox(bottom.." "..top)
	AsynCall(function (x) tabbkg:ActivePage("MydownloadPage") end)
end

function OnActiveTabChanged(self,eventName,newID,oldID)
	--XLMessageBox("tabchanged")
	local tabbkg = self:GetOwner():GetUIObject("tabbkg")
	tabbkg:ActivePage(newID)
	return true
end

