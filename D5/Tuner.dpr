program Tuner;

uses
  Forms,
  Main in 'Main.pas' {frmMain},
  utFreqCalc in 'utFreqCalc.pas';

{$R *.RES}

begin
  Application.Initialize;
  Application.CreateForm(TfrmMain, frmMain);
  Application.Run;
end.
