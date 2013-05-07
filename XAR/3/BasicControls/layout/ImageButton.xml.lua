local function SetStatus(self, newState, forceupdate)
    local attr = self:GetAttribute()
    if forceupdate or newState ~= attr.Status then
        local ownerTree = self:GetOwner()
        local bkg = self:GetControlObject("button.bkg")

        if newState == 1 then
            bkg:SetResID(attr.NormalBkgID)
        elseif newState == 2 then
            bkg:SetResID(attr.HoverBkgID)
        elseif newState == 3 then
            bkg:SetResID(attr.DownBkgID)
        elseif newState == 4 then
            bkg:SetResID(attr.DisableBkgID)
        end

        attr.Status = newState
    end
end

function UpdateUI(self)
    local attr = self:GetAttribute()
	local bkg = self:GetControlObject("button.bkg")

	if attr.Status == 1 then
		bkg:SetResID(attr.NormalBkgID)
	elseif attr.Status == 2 then
		bkg:SetResID(attr.HoverBkgID)
	elseif attr.Status == 3 then
		bkg:SetResID(attr.DownBkgID)
	elseif attr.Status == 4 then
		bkg:SetResID(attr.DisableBkgID)
	end

end

function OnLButtonDown(self)
	local img = self;
    self = self:GetOwnerControl()
	local attr = self:GetAttribute()
	if attr.Status ~= 4 and attr.Status ~= 3 then
		img:SetCaptureMouse(true)
		attr.Capture = true
		SetStatus(self, 3)
	end
end



function OnLButtonUp(self, x, y, flags)
	local img = self
	self = self:GetOwnerControl()
	local attr = self:GetAttribute()
	local status = attr.Status
	
	if attr.Capture then
		img:SetCaptureMouse(false)
		attr.Capture = false
	end
	if status ~= 4 then
		if status ~= 2 then
			SetStatus(self, 2)
		end
		local left, top, right, bottom = self:GetObjPos()
		if x >= 0 and x <= right - left and y >= 0 and y <= bottom - top then
			self:FireExtEvent("OnClick")
		end
	end
end

function OnRButtonDown(self)
	self = self:GetOwnerControl()
    local attr = self:GetAttribute()
	if attr.EnableRightBtnClick == false then
		return
	end

	OnLButtonDown(self)
end

function OnRButtonUp(self, x, y, flags)
	self = self:GetOwnerControl()
	local attr = self:GetAttribute()
	local status = attr.Status

	if attr.EnableRightBtnClick == false then
		return
	end
	
	OnLButtonUp(self,x,y,flags)
end

function OnMouseMove(self, x, y)
	self = self:GetOwnerControl()
    local left, top, right, bottom = self:GetObjPos()
    local width, height = right - left, bottom - top
    
    local attr = self:GetAttribute()
    if attr.Status ~= 4 then
        if attr.HandHover then
            self:SetCursorID("IDC_HAND")
        end
		SetStatus(self, 2)
    end
end


function OnMouseLeave(self)
	self = self:GetOwnerControl()
    local attr = self:GetAttribute()
    if attr.Status ~= 4 then
		if attr.HandHover then
            self:SetCursorID("IDC_ARROW")
        end
        SetStatus(self, 1)
    end
end


function OnBind(self)
	
    local attr = self:GetAttribute()
    local bkg = self:GetControlObject("button.bkg")
	if attr.NormalBkgID then
		bkg:SetResID(attr.NormalBkgID)
	end
end

function SetBitmap( self, nor, hover, down, disable )
	local attr = self:GetAttribute()
	attr.NormalBkgID = ""
	if nor ~= nil then
		attr.NormalBkgID = nor
	end
	attr.DownBkgID = ""
	if down ~= nil then
		attr.DownBkgID = down
	end
	attr.DisableBkgID = ""
	if disable ~= nil then
		attr.DisableBkgID = disable
	end
	attr.HoverBkgID = ""
	if hover ~= nil then
		attr.HoverBkgID = hover
	end
	SetStatus(self, attr.Status, true)
end

function GetBitmap(self)
    local attr = self:GetAttribute()
    return attr.NormalBkgID, attr.HoverBkgID, attr.DownBkgID, attr.DisableBkgID
end


function SetButtonAlpha(self, alpha)
	local bkgObj = self:GetControlObject("button.bkg")
	bkgObj:SetAlpha(alpha)
end

function OnEnableChange(self, enable)
	local attr = self:GetAttribute()
	if attr.Status == 4 and not enable or
		attr.Status ~= 4 and enable then
		return
	end

	local status = 1
	if not enable then
		status = 4
	end

	SetStatus(self, status)
end

function OnVisibleChange(self, visible)
	
	-- self:SetVisible(visible)
	self:SetChildrenVisible(visible)	
end