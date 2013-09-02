#include "SettingsDlg.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

SettingsDlg::SettingsDlg( )
{
}

SettingsDlg::~SettingsDlg()
{
}

void SettingsDlg::OnButtonOkClick()
{
	close();
}

void SettingsDlg::OnButtonCancelClick()
{
	close();
}

SettingsDlg::SettingsDlg( QDialog* parent )
{
    Create(parent);
}

bool SettingsDlg::Create( QDialog* parent )
{
    _btnOk = NULL;
	_btnCancel = NULL;
    //QDialog::Create( parent, id, caption, pos, size, style );
    CreateControls();
    _icon = new QIcon("vorbital.ico");
    setWindowIcon(*_icon);
    return true;
}

void SettingsDlg::CreateControls()
{
	SettingsDlg* itemDialog1 = this;

    QVBoxLayout* itemBoxSizer2 = new QVBoxLayout();

    QHBoxLayout* itemBoxSizer3 = new QHBoxLayout();
    itemBoxSizer2->addLayout(itemBoxSizer3);

    itemBoxSizer3->addSpacing(2);

	_btnOk = new QPushButton( "OK", itemDialog1 );
	_btnOk->setToolTip("Accept settings.");
    itemBoxSizer3->addWidget(_btnOk);

    itemBoxSizer3->addSpacing(6);

    _btnCancel = new QPushButton( "Cancel", itemDialog1 );
	_btnOk->setToolTip("Ignore changes.");
    itemBoxSizer3->addWidget(_btnCancel);

    itemBoxSizer3->addSpacing(6);

    itemDialog1->setLayout(itemBoxSizer2);

    connect(_btnOk, SIGNAL(released()), this, SLOT(onButtonOkClick()));
    connect(_btnCancel, SIGNAL(released()), this, SLOT(onButtonCancelClick()));
}
