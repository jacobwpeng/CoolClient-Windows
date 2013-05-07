function OnPopupMenu()

end

function Menu_Item_OnInitControl(self)
	local attr = self:GetAttribute()
	local icon = self:GetControlObject("icon")
	local text = self:GetControlObject("text")
	icon:SetResID(attr.Icon)
	text:SetText(attr.Text)
end

--用来控制鼠标移进菜单项区域
function Menu_Item_OnMouseEnter(self)
	local fill = self:GetControlObject("bkg")
	fill:SetVisible(true)
end
--用来控制鼠标移出菜单项区域
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