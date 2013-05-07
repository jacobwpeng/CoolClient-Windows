function SetButtonActive(self, button, active)
	local attr = self:GetAttribute()
	if active then
		button:SetNormalBkgTexture(attr.ActiveBtnBkgNormal)
		button:SetHoverBkgTexture(attr.ActiveBtnBkgHover)
		button:SetDownBkgTexture(attr.ActiveBtnBkgDown)
		button:SetDisableBkgTexture(attr.ActiveBtnBkgDisable)
		button:SetTextColorID("system.black")
	else
		button:SetNormalBkgTexture(attr.BtnBkgNormal)
		button:SetHoverBkgTexture(attr.BtnBkgHover)
		button:SetDownBkgTexture(attr.BtnBkgDown)
		button:SetDisableBkgTexture(attr.BtnBkgDisable)
		button:SetTextColorID("system.white")
	end
end

function SetText(self,id,strText)
	local attr = self:GetAttribute()
	if not attr or not id then
		return
	end
	
	local obj = self:GetControlObject(id)
	if not obj then
		return
	end
	
	obj:SetText(strText)
end

--	active:bool 是否激活
function AddTabItem(self, id, text, icon, active)

	local objExist = self:GetControlObject(id)
	if objExist ~= nil then
		return
	end
	
	local attr = self:GetAttribute()

	local bkgObj = self:GetControlObject("bkg")
	local objFactory = XLGetObject("Xunlei.UIEngine.ObjectFactory")
	local btn = objFactory:CreateUIObject(id, "Thunder.ImageTextButton")
	
	local btnAttr = btn:GetAttribute()
	btnAttr.IconLeftPos = attr.IconLeftPos
	btnAttr.IconTopPos = attr.IconTopPos
	btnAttr.TextLeftPos = attr.TextLeftPos
	btnAttr.TextTopPos = attr.TextTopPos
	btnAttr.IconSize = attr.IconSize
	btnAttr.IconBitmapID = icon
	btnAttr.Text = text

	btnAttr.TextFontID = attr.TextFontID
	btnAttr.TextColorID = attr.TextColorID
	
	local pos = 0
	local btnCount = bkgObj:GetChildCount()
	--XLMessageBox(btnCount)
	if btnCount > 0 then
		local lastBtn = bkgObj:GetChildByIndex(btnCount - 1)
		local left, top, right, bottom = lastBtn:GetObjPos()
		pos = right + attr.ButtonInternalSpace
	end

	local function OnButtonClick(btn)
		local id = btn:GetID()
		if id == attr.CurrentActiveTab then
			return
		end
		self:SetActiveTab(id,true)
	end
	btn:AttachListener("OnButtonClick", true, OnButtonClick)
	
	bkgObj:AddChild(btn)

	btn:SetObjPos(pos, 0, pos + attr.ButtonWidth, 0 + attr.ButtonHeight)

	if active == true or
		(active == nil and attr.CurrentActiveTab == nil) then
		self:SetActiveTab(id, true)
		--XLMessageBox("if true")
	else
		--XLMessageBox("if else")
		SetButtonActive(self, btn, true)
	end
	
	if id ~= "MydownloadPage" then
		local close = objFactory:CreateUIObject(id.."close", "Thunder.ImageButton.Ctrl")
		local closeAttr = close:GetAttribute()
		closeAttr.NormalBkg = "bitmap.tab.close.normal"
		closeAttr.HoverBkg = "bitmap.tab.close.hover"
		closeAttr.DownBkg = "bitmap.tab.close.lbuttondown"
		local function OnClose(close)
			local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
			local hostwnd = hostwndManager:GetHostWnd("MainFrame")
			local userData = hostwnd:GetUserData()
			userData[id] = false
			AsynCall(function() self:RemoveTabItem(id,"MydownloadPage") end)
		end
		close:AttachListener("OnClick", true, OnClose)
		btn:AddChild(close)
		close:SetObjPos2(attr.ButtonWidth - 20,5,14,14)
	end
	
end


function RemoveTabItem(self, remove_id, active_id)
	local btn = self:GetControlObject(remove_id)
	if btn == nil then
		return
	end
	local bkgObj = self:GetControlObject("bkg")
	local attr = self:GetAttribute()
	local pos = 0
	local count = bkgObj:GetChildCount()
	for i = 0, count - 1 do
		local child = bkgObj:GetChildByIndex(i)
		local childID = child:GetID()
		if childID ~= remove_id then
			local childLeft, childTop, childRight, childBottom = child:GetObjPos()
			child:SetObjPos(pos, childTop, pos + attr.ButtonWidth, childBottom)
			pos = pos + attr.ButtonWidth + attr.ButtonInternalSpace
		end
	end
	bkgObj:RemoveChild(btn)
	--XLMessageBox(active_id)
	self:SetActiveTab(active_id,false)
	self:FireExtEvent("OnActiveTabChanged",active_id)
end

function SetActiveTab(self, tabID, fireEvent)
	--XLMessageBox("SetActiveTab")
	local attr = self:GetAttribute()
	if attr.CurrentActiveTab == tabID then
		return
	end

	local btn = self:GetControlObject(tabID)
	if btn == nil then
		return
	end
	SetButtonActive(self, btn, true)
	
	local pre_active = attr.CurrentActiveTab
	if pre_active ~= nil then
		local btn = self:GetControlObject(pre_active)
		if btn ~= nil then
			SetButtonActive(self, btn, false)
		end
	end
	
	attr.CurrentActiveTab = tabID
	if fireEvent ~= false then
		self:FireExtEvent("OnActiveTabChanged", tabID, pre_active)
		--XLMessageBox("fireEvent")
	end
end


function OnInitTabCtrlBkg(self)
	local attr = self:GetAttribute()
	local bkg = self:GetControlObject("bkg")
	bkg:SetTextureID(attr.BorderTexture)
end

function AddPage(self,classID,instanceID)
	local attr = self:GetAttribute()
	if attr.Pages == nil then
		attr.Pages = {}
	end
	for k,v in pairs(attr.Pages) do
		if v.id == instanceID then
			return false
		end
	end
	attr.Pages[#(attr.Pages)+1] = {class=classID,id=instanceID,obj=nil}
	return true
end

function ActivePage(self,instanceID)
	local attr = self:GetAttribute()
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local hostwnd = hostwndManager:GetHostWnd("MainFrame")
	local userData = hostwnd:GetUserData()
	if attr.Pages == nil then
		return false
	end
	
	for k,v in pairs(attr.Pages) do
		if v.id == instanceID then
			if attr.selectPage == v then
				return true
			end
			
			local xarManager = XLGetObject("Xunlei.UIEngine.XARManager")
			local xarFactory = xarManager:GetXARFactory()
			local obj = xarFactory:CreateUIObject(v.id,v.class)
			if obj == nil then
				return false;
			end
			
			v.obj = obj
			
			if attr.selectPage ~= nil then
				if userData[(attr.selectPage.id)] == false then
					self:GetControlObject("bkg"):RemoveChild(self:GetControlObject(attr.selectPage.id))
				else
					--self:GetControlObject(attr.selectPage.id):SetVisible(false,true)
					self:GetControlObject(attr.selectPage.id):SetObjPos2(1920,1080,"father.width","father.height")
				end	
				attr.selectPage.obj = nil
			end
			attr.selectPage = v
			if v.id == "MydownloadPage" and userData.MydownloadPage == false then
				self:GetControlObject("bkg"):AddChild(v.obj)
				userData.MydownloadPage = true
				return true
			end
			if userData[(v.id)] == true then
				--self:GetControlObject(v.id):SetVisible(true,true)
				self:GetControlObject(v.id):SetObjPos2(0,0,"father.width","father.height")
			else 
				self:GetControlObject("bkg"):AddChild(v.obj)
			end
			return true
		end
	end
	
	return false
end