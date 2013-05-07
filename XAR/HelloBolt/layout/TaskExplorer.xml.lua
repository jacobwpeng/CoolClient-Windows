--前面需要定义几个local funciton  用来表示不同的按钮被点击需要做的处理
local function ItemSearchSelected(self)
	local header = self:GetOwner():GetUIObject("tabHeader")
	local tabbkg = self:GetOwner():GetUIObject("tabbkg")
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local hostwnd = hostwndManager:GetHostWnd("MainFrame")
	local userData = hostwnd:GetUserData()
	if userData.SearchPage == false then
		header:AddTabItem("SearchPage","搜索种子","bitmap.tab.search", false)
		tabbkg:AddPage("SearchPage","SearchPage")
	end	 
	header:SetActiveTab("SearchPage", true)
	AsynCall(function() userData.SearchPage = true end)
end

local function OnStatusChange(self, fireEvent)
	local attr = self:GetAttribute()
	local bkg = self:GetControlObject("bkg")
	local icon = self:GetControlObject("icon")
	local text = self:GetControlObject("text")
	
	if attr.Status == "hover" then
		bkg:SetAlpha(255)
		bkg:SetResID("bitmap.pluginitem.hover")
	elseif attr.Status == "leave" then
		local counter1 = SetTimer(function() bkg:SetAlpha(bkg:GetAlpha()-25) end, 10)
		local counter2 = SetOnceTimer(function() KillTimer(counter1) end, 150)
		local counter3 = SetOnceTimer(function() bkg:SetAlpha(0) text:SetTextColorResID("system.black") icon:SetResID(attr.IconNormal) end, 150)
	elseif attr.Status == "active" then
		bkg:SetAlpha(255)
		bkg:SetResID("bitmap.pluginitem.active")
		icon:SetResID(attr.IconActive)
		text:SetTextColorResID("system.white")
		if fireEvent == true then
			self:FireExtEvent("OnSelect")
		end
	end
end

function OnSelectChange(self)
--[[
	local selfAttr = self:GetAttribute()
	local owner = self:GetOwnerControl()
	local ownerAttr = owner:GetAttribute()
	local id = self:GetID()
	local header = self:GetOwner():GetUIObject("tabHeader")
	local tabbkg = self:GetOwner():GetUIObject("tabbkg")
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local hostwnd = hostwndManager:GetHostWnd("MainFrame")
	local userData = hostwnd:GetUserData()
	
	if id == "item.download" then
		header:SetActiveTab("MydownloadPage",true)
	elseif id == "item.search" then
		if userData.SearchPage == false then
			header:AddTabItem("SearchPage","搜索种子","bitmap.tab.search", false)
			tabbkg:AddPage("SearchPage","SearchPage")
		end	 
		header:SetActiveTab("SearchPage", true)
		AsynCall(function() userData.SearchPage = true end)
		SelectChanged(self,"normal")
		--XLMessageBox("setfinish")
	end
	SelectChanged(self,"active")
	if id ~= ownerAttr.CurrentChosen and id ~= "item.search" then
		local prelayout = owner:GetControlObject(ownerAttr.CurrentChosen)
		SelectChanged(prelayout, "leave")
		ownerAttr.CurrentChosen = id
	end
	]]
	--这里只用对两个按钮状态进行调度，之后再调用相应的函数
	local selfAttr = self:GetAttribute()
	local owner = self:GetOwnerControl()
	local ownerAttr = owner:GetAttribute()
	local id = self:GetID()
	
	local preItem = owner:GetControlObject(ownerAttr.CurrentChosen)
	local curItem = owner:GetControlObject(id)
	
	preItem:GetAttribute().Status = "leave"
	OnStatusChange(preItem,false)
	
	curItem:GetAttribute().Status = "active"
	OnStatusChange(curItem,false)
	
	ownerAttr.CurrentChosen = id
	
	return true
end

function OnItemInit(self)
	local icon = self:GetControlObject("icon")
	local text = self:GetControlObject("text")
	local attr = self:GetAttribute()
	icon:SetResID(attr.IconNormal)
	text:SetText(attr.Text)
	if self:GetID() == "item.download" then
		attr.Status = "active"
		--self:FireExtEvent("OnStatusChange")
		local bkg = self:GetControlObject("bkg")
		bkg:SetAlpha(255)
		bkg:SetResID("bitmap.pluginitem.active")
		icon:SetResID(attr.IconActive)
		text:SetTextColorResID("system.white")
	end
end

function OnMouseEnter(self)
	local ownerAttr = self:GetOwnerControl():GetAttribute()
	local selfAttr = self:GetAttribute()
	if ownerAttr.CurrentChosen ~= self:GetID() then
		selfAttr.Status = "hover"
		OnStatusChange(self)
	end
end

function OnMouseLeave(self)
	--XLMessageBox(self:GetID())
	local ownerAttr = self:GetOwnerControl():GetAttribute()
	local selfAttr = self:GetAttribute()
	if self:GetID() ~= ownerAttr.CurrentChosen then
		selfAttr.Status = "leave"
		OnStatusChange(self)
	end
end

function OnLButtonDown(self)
	local ownerAttr = self:GetOwnerControl():GetAttribute()
	local selfAttr = self:GetAttribute()
	local id = self:GetID()
	
	if id == "item.search" then
		ItemSearchSelected(self)
		return 
	end
	
	if ownerAttr.CurrentChosen ~= id then
		selfAttr.Status = "active"
		OnStatusChange(self,true)
	end
end