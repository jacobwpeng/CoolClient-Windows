function OnPopupMenu()

end

function Menu_Item_OnInitControl(self)
	local attr = self:GetAttribute()
	local icon = self:GetControlObject("icon")
	local text = self:GetControlObject("text")
	icon:SetResID(attr.Icon)
	text:SetText(attr.Text)
end

function Menu_Item_OnMouseEnter(self)
	local fill = self:GetControlObject("bkg")
	fill:SetVisible(true)
end

function Menu_Item_OnMouseLeave(self)
	local fill = self:GetControlObject("bkg")
	fill:SetVisible(false)
end

function Menu_Item_OnLButtonDown(self)
	self:FireExtEvent("OnMenuItemClick")
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local menu = hostwndManager:GetHostWnd("Thunder.MenuHostWnd.Instance")
	menu:EndMenu()
end

function SetText(self,text)
	local txtObj = self:GetControlObject("text")
	txtObj:SetText(text)
end

function OnMainMenuConfigClick(self)
	local header = self:GetOwner():GetUIObject("tabHeader")
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local hostwnd = hostwndManager:GetHostWnd("MainFrame")
	local treeManager = XLGetObject("Xunlei.UIEngine.TreeManager")                   
	local tree = treeManager:GetUIObjectTree("MainObjectTree")
	local tabbkg= tree:GetUIObject("tabbkg")
	local header = tree:GetUIObject("tabHeader")
	local userData = hostwnd:GetUserData()
	if userData.ConfigPage == false then
		header:AddTabItem("ConfigPage","配置中心","bitmap.tab.config", false)
		tabbkg:AddPage("ConfigPage","ConfigPage")
	end	 
	header:SetActiveTab("ConfigPage", true)
	AsynCall(function() userData.ConfigPage = true end)
end

function OnMainMenuExitClick(self)
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local hostwnd = hostwndManager:GetHostWnd("MainFrame")
	hostwnd:SetVisible(false)
	local coolClientProxy = XLGetObject('CoolDown.CoolClient.Proxy')
	coolClientProxy:StopClient()
	os.exit()
end