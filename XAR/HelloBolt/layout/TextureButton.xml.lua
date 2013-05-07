--初始化纹理按钮，添加纹理和按钮文字
function OnInitControl(self)
	local text = self:GetControlObject("texturebtn.text")
	local bkg = self:GetControlObject("texturebtn.bkg")
	local attr = self:GetAttribute()
	
	text:SetText(attr.Text)
	text:SetTextColorResID(attr.TextColor)
	bkg:SetResID(attr.NormalBkg)
end

--鼠标进入按钮后更换背景
function OnMouseEnter(self)
	local attr = self:GetAttribute()
	local ctrl = self:GetControlObject("texturebtn.bkg")
	ctrl:SetResID(attr.HoverBkg)
end

--鼠标按下的效果
function OnLButtonDown(self)
	local attr = self:GetAttribute()
	local ctrl = self:GetControlObject("texturebtn.bkg")
	ctrl:SetResID(attr.DownBkg)
	self:FireExtEvent("OnClick")
end
function OnLButtonUp(self)
	local attr = self:GetAttribute()
	local ctrl = self:GetControlObject("texturebtn.bkg")
	ctrl:SetResID(attr.NormalBkg)
end
--鼠标离开后背景替换为默认
function OnMouseLeave(self)
	local attr = self:GetAttribute()
	local ctrl = self:GetControlObject("texturebtn.bkg")
	ctrl:SetResID(attr.NormalBkg)
end

function OnPosChange(self,oldLeft,oldTop,oldRight,oldBottom,newLeft,newTop,newRight,newBottom)
	
	return 0,true
end

function SetText(self,text)
	local txtObj = self:GetControlObject("texturebtn.text")
	txtObj:SetText(text)
end